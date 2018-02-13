#include <windows.h>
#include<stdio.h>

//OpenGL headers
#include<GL/glew.h>

#include <gl/GL.h>
#include <gl/GLU.h>

#include "vmath.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// import libraries
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

using namespace vmath;

enum
{
	VDG_ATTRIBUTE_VERTEX = 0,
	VDG_ATTRIBUTE_COLOR = 1,
	VDG_ATTRIBUTE_NORMAL = 2,
	VDG_ATTRIBUTE_TEXTURE = 3,
};


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullscreen = false;

int printOpenGlExtentions(void);
void update(void);

FILE *gpFile = NULL;

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

void uninitialize(void);

GLuint gVao_triangle;
GLuint gVao_quads;

GLuint gVbo;
GLuint gVbo_quads;

GLuint gVboColorQuods;

GLuint gMVPUniform;
//
mat4 gPerspectiveProjectionMatrix;

GLfloat angleTriangle = 0.0f, angleQuads = 0.0f;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	void initialize(void);
	void update(void);
	void display(void);
	void uninitialize(void);

	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("OpenGL");
	bool bDone = false;

	if (fopen_s(&gpFile, "log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("********* Unable to create log file. \n Exitting ..."), TEXT("File Can Not Created"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "***** File created successfull.\n");
	}


	// WNDCLASSEX initialization
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// register WNDCLASSEX
	RegisterClassEx(&wndclass);

	// create window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		TEXT("PP:Two Rotating Shapes"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	// OpenGL initialization
	initialize();

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// message loop
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == true)
			{
				if (gbEscapeKeyIsPressed == true)
					bDone = true;
			}
			update();
			display();
		}
	}

	uninitialize();
	return((int)msg.wParam);
}

// WndProc()
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

	void resize(int, int);
	void ToggleFullscreen(void);
	void uninitialize(void);

	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			gbActiveWindow = true;
		else
			gbActiveWindow = false;
		break;
	case WM_ERASEBKGND:
		return(0);
	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			if (gbEscapeKeyIsPressed == false)
				gbEscapeKeyIsPressed = true;
			break;
		case 0x46: //for 'f' or 'F'
			if (gbFullscreen == false)
			{
				ToggleFullscreen();
				gbFullscreen = true;
			}
			else
			{
				ToggleFullscreen();
				gbFullscreen = false;
			}
			break;
		default:
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		break;
	case WM_CLOSE:
		uninitialize();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullscreen(void)
{
	MONITORINFO mi;

	if (gbFullscreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}
}

void initialize(void)
{
	void resize(int, int);


	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;


	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	// PIXELFORMATDESCRIPTOR Initialization
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	ghdc = GetDC(ghwnd);

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == false)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (wglMakeCurrent(ghdc, ghrc) == false)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	//**************************************** Vertex shader **********************************************
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	const GLchar *vertexShaderSourceCode =
		"#version 430 core"\
		"\n"\
		"in vec4 vColor;"\
		"in vec4 vPosition;"\
		"out vec4 out_color;"\
		"uniform mat4 u_mvp_matrix;"\
		"void main(void)" \
		"{" \
		"gl_Position=u_mvp_matrix * vPosition;"
		"out_color=vColor;"
		"}";

	glShaderSource(gVertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);

	//******************* Compile Vertex shader 
	glCompileShader(gVertexShaderObject);

	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char *szInfoLog = NULL;
	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "***** Vertex Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	//**************************************** Fragment shader **********************************************
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar *fragmentShaderSourceCode =
		"#version 430 core"\
		"\n"\
		"in vec4 out_color;"
		"out vec4 FragColor;"
		"void main(void)" \
		"{" \
		"FragColor=out_color;"//vec4(1.0,1.0,1.0,1.0);"\ 
		"}";

	glShaderSource(gFragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);

	//******************* Compile fragment shader 

	glCompileShader(gFragmentShaderObject);
	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "***** Fragment Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	//**************************************** Shader program attachment **********************************************
	// Code from sir

	gShaderProgramObject = glCreateProgram();

	// attach vertex shader to shader program
	glAttachShader(gShaderProgramObject, gVertexShaderObject);

	// attach fragment shader to shader program
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	//glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_VERTEX, "vPosition");
	//glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_VERTEX, "vColor");

	//**************************************** Link Shader program **********************************************
	glLinkProgram(gShaderProgramObject);
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength>0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	//**************************************** END Link Shader program **********************************************

	gMVPUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
//************************************ Pyramid ********************************************************
	const GLfloat triangleVertices[] =
	{ 
		0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f
	};

	glGenVertexArrays(1, &gVao_triangle);
	glBindVertexArray(gVao_triangle);

	glGenBuffers(1, &gVbo);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

//******************************** Pyramid Color 
	const GLfloat colorVertices[] =
	{
		1.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,0.0f,1.0f,

		1.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,0.0f,1.0f,

		1.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,0.0f,1.0f,

		1.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,0.0f,1.0f
	};

	glBindVertexArray(gVao_triangle);
	glGenBuffers(1, &gVbo);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorVertices), colorVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(VDG_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

//************************************ Quods ********************************************************

	const GLfloat quadsVertices[] =
	{ 
		1.0f, 1.0f, -1.0f,  //right-top corner of top face
		-1.0f, 1.0f, -1.0f, //left-top corner of top face
		-1.0f, 1.0f, 1.0f, //left-bottom corner of top face
		1.0f, 1.0f, 1.0f, //right-bottom corner of top face

		1.0f, -1.0f, -1.0f, //right-top corner of bottom face
		-1.0f, -1.0f, -1.0f, //left-top corner of bottom face
		-1.0f, -1.0f, 1.0f, //left-bottom corner of bottom face
		1.0f, -1.0f, 1.0f, //right-bottom corner of bottom face

		1.0f, 1.0f, 1.0f, //right-top corner of front face
		-1.0f, 1.0f, 1.0f, //left-top corner of front face
		-1.0f, -1.0f, 1.0f, //left-bottom corner of front face
		1.0f, -1.0f, 1.0f, //right-bottom corner of front face

		1.0f, 1.0f, -1.0f, //right-top of back face
		-1.0f, 1.0f, -1.0f, //left-top of back face
		-1.0f, -1.0f, -1.0f, //left-bottom of back face
		1.0f, -1.0f, -1.0f, //right-bottom of back face

		1.0f, 1.0f, -1.0f, //right-top of right face
		1.0f, 1.0f, 1.0f, //left-top of right face
		1.0f, -1.0f, 1.0f, //left-bottom of right face
		1.0f, -1.0f, -1.0f, //right-bottom of right face

		-1.0f, 1.0f, 1.0f, //right-top of left face
		-1.0f, 1.0f, -1.0f, //left-top of left face
		-1.0f, -1.0f, -1.0f, //left-bottom of left face
		-1.0f, -1.0f, 1.0f, //right-bottom of left face
	};

	glGenVertexArrays(1, &gVao_quads);
	glBindVertexArray(gVao_quads);

	glGenBuffers(1, &gVbo_quads);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_quads);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadsVertices), quadsVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

//************************************ Quods color
	GLfloat colorQuadsVertices[] =
	{
		1.0f, 0.0f, 0.0f, //RED
		1.0f, 0.0f, 0.0f, //RED
		1.0f, 0.0f, 0.0f, //RED
		1.0f, 0.0f, 0.0f, //RED

		1.0f, 0.0f, 1.0f, //MAGENTA
		1.0f, 0.0f, 1.0f, //MAGENTA
		1.0f, 0.0f, 1.0f, //MAGENTA
		1.0f, 0.0f, 1.0f, //MAGENTA

		0.0f, 1.0f, 1.0f, //CYAN
		0.0f, 1.0f, 1.0f, //CYAN
		0.0f, 1.0f, 1.0f, //CYAN
		0.0f, 1.0f, 1.0f, //CYAN

		0.0f, 0.0f, 1.0f, //BLUE
		0.0f, 0.0f, 1.0f, //BLUE
		0.0f, 0.0f, 1.0f, //BLUE
		0.0f, 0.0f, 1.0f, //BLUE

		0.0f, 1.0f, 0.0f, //GREEN
		0.0f, 1.0f, 0.0f, //GREEN
		0.0f, 1.0f, 0.0f, //GREEN
		0.0f, 1.0f, 0.0f, //GREEN

		1.0f, 1.0f, 0.0f, //YELLOW
		1.0f, 1.0f, 0.0f, //YELLOW
		1.0f, 1.0f, 0.0f, //YELLOW
		1.0f, 1.0f, 0.0f, //YELLOW
	};

	glBindVertexArray(gVao_quads);
	glGenBuffers(1, &gVboColorQuods);
	glBindBuffer(GL_ARRAY_BUFFER, gVboColorQuods);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorQuadsVertices), colorQuadsVertices, GL_STATIC_DRAW);

	
//	glVertexAttrib3f(VDG_ATTRIBUTE_COLOR, 1.0f, 0.0f, 0.0f);
	glVertexAttribPointer(VDG_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

//***********************************************************************************
	printOpenGlExtentions();

	glShadeModel(GL_SMOOTH);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glEnable(GL_CULL_FACE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	gPerspectiveProjectionMatrix = mat4::identity();

	resize(WIN_WIDTH, WIN_HEIGHT);
}


void resize(int width, int height)
{
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	gPerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}

void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);

	//******************************* Triangle block **********************************
	mat4 	modelViewMatrix = mat4::identity();  //initialize model view matrix
	modelViewMatrix = translate(-2.0f, 0.0f, -6.0f); //translate function return matrix with translate parameter

	mat4  rotationMatrix = mat4::identity();
	rotationMatrix = rotate(angleTriangle, 0.0f, 1.0f, 0.0f);

	modelViewMatrix = modelViewMatrix*rotationMatrix;

	mat4  modelViewProjectionMatrix = mat4::identity();

	modelViewProjectionMatrix = gPerspectiveProjectionMatrix*modelViewMatrix;

	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glBindVertexArray(gVao_triangle);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 12);

	glBindVertexArray(0);

	//******************************* Quads block **********************************

	modelViewMatrix = mat4::identity();  //initialize model view matrix
	modelViewMatrix = translate(2.0f, 0.0f, -8.0f); //translate function return matrix with translate parameter

	rotationMatrix = mat4::identity();
	rotationMatrix = rotate(angleQuads, angleQuads, angleQuads);

	modelViewMatrix = modelViewMatrix*rotationMatrix;

	modelViewProjectionMatrix = mat4::identity();
	modelViewProjectionMatrix = gPerspectiveProjectionMatrix*modelViewMatrix;

	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glBindVertexArray(gVao_quads);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);



	//*****************************************************************************
	glBindVertexArray(0);

	glUseProgram(0);

	SwapBuffers(ghdc);

}


void uninitialize(void)
{

	if (gbFullscreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}

	if (gVao_triangle)
	{
		glDeleteVertexArrays(1, &gVao_triangle);
		gVao_triangle = 0;
	}

	if (gVao_quads)
	{
		glDeleteVertexArrays(1, &gVao_quads);
		gVao_quads = 0;
	}

	if (gVbo)
	{
		glDeleteBuffers(1, &gVbo);
		gVbo = 0;
	}

	glDetachShader(gShaderProgramObject, gVertexShaderObject);
	glDetachShader(gShaderProgramObject, gFragmentShaderObject);


	glDeleteShader(gVertexShaderObject);
	gVertexShaderObject = 0;

	glDeleteShader(gFragmentShaderObject);
	gFragmentShaderObject = 0;

	glDeleteProgram(gShaderProgramObject);
	gShaderProgramObject = 0;

	glUseProgram(0);

	wglMakeCurrent(NULL, NULL);

	wglDeleteContext(ghrc);
	ghrc = NULL;

	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;

	if (gpFile)
	{
		fprintf(gpFile, "Log file closing successfully\n");
		fclose(gpFile);
		gpFile = NULL;
	}

	DestroyWindow(ghwnd);
}

int printOpenGlExtentions(void)
{
	GLint num;

	glGetIntegerv(GL_NUM_EXTENSIONS, &num);

	for (int i = 0; i < num; i++)
	{
		fprintf(gpFile, "Shader Program Link Log : %s\n", glGetStringi(GL_EXTENSIONS, i));
	}
	return 0;
}

void update(void)
{
	//code
	angleTriangle = angleTriangle + 0.1f;
	if (angleTriangle >= 360.0f)
		angleTriangle = 0.0f;

	angleQuads = angleQuads + 0.1f;
	if (angleQuads >= 360.0f)
		angleQuads = 0.0f;
}