// First 3D_Shapes.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#includ  <stdlib.h> //Needed for "exit" function
#includ  <GL/glut.h>
using namespace std;

//Called when a key is pressed
void handlekeypress(unsigned char key, //The key that was pressed
	int x, int y)
{    // The Current mouse coordinates
	switch (key)
	{
	case 27: //Escape key
		exit(o); //Exit the program
	}
}

//Initializes 3D rendering
void initRendering()
{
	//Makes 3D drawing work when something is infront of something
	glEnable(GL_DEPTH_TEST);
}

//Called when the window is resized
void handle Resize(int w, int h);
{
	//Tell OpenGL how to convert from coordinates to pixel values
	gllViewport(o, o, w, h);

	glMatrixMode(GL_PROJECTION); //Switch to setting the camera perspective

	//Set the camera perspective
	glLoadIdentity(); //Reset the camera
	gluPersrective(45.0,        //The camera angle
		(double)w / (double)h, //The width-to-height
		1.0,                   //The near z clipping coo
		200.0);               //The far z clipping coo
}

int main(int argc, char** argv)
{
	//Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowsSize(400, 400);//set the window size

	//Creat the window
	glutCreatWindow("Basic Shapes - Demo Video");
	initRendering(); //Initilize rendering

	//Set handler functions for drawing, keypresses, and window reshape
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handlekeypress);
	glutReshapeFunc(handleresize);

	glutMainLoop(); //Start the main loop. glutMainLoop dosen't return.
	return o; //This line is never reached
}

