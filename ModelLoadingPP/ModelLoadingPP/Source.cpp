#include<Windows.h>
#include<iostream>
#include<string>
#include<vector>
#include<stdlib.h>
#include<fstream>
using namespace std;
typedef struct Model {
	long int vertices;
	long int positions;
	long int texels;
	long int normals;
	long int faces;
}Model;

// 1
Model getOBJinfo(string fp)
{
	// 2
	Model model = { 0 };

	// 3
	// Open OBJ file
	ifstream inOBJ;
	inOBJ.open(fp);
	if (!inOBJ.good())
	{
		cout << "ERROR OPENING OBJ FILE" << endl;
		exit(1);
	}

	// 4
	// Read OBJ file
	while (!inOBJ.eof())
	{
		// 5
		string line;
		getline(inOBJ, line);
		string type = line.substr(0, 2);

		// 6
		if (type.compare("v ") == 0)
			model.positions++;
		else if (type.compare("vt") == 0)
			model.texels++;
		else if (type.compare("vn") == 0)
			model.normals++;
		else if (type.compare("f ") == 0)
			model.faces++;
	}

	// 7
	model.vertices = model.faces * 3;

	// 8
	// Close OBJ file
	inOBJ.close();

	// 9
	return model;
}
void extractOBJdata(string fp, float** positions, float** texels, float** normals, long int** faces)
{
	// Counters
	long int p = 0;
	long int t = 0;
	long int n = 0;
	long int f = 0;

	// Open OBJ file
	ifstream inOBJ;
	inOBJ.open(fp);
	if (!inOBJ.good())
	{
		cout << "ERROR OPENING OBJ FILE" << endl;
		exit(1);
	}

	// Read OBJ file
	while (!inOBJ.eof())
	{
		string line;
		getline(inOBJ, line);
		string type = line.substr(0, 2);

		// Positions
		if (type.compare("v ") == 0)
		{
			// 1
			// Copy line for parsing
			char* l = new char[line.size() + 1];
			memcpy(l, line.c_str(), line.size() + 1);

			// 2
			// Extract tokens
			strtok(l, " ");
			for (long int i = 0; i<3; i++)
				positions[p][i] = atof(strtok(NULL, " "));

			// 3
			// Wrap up
			delete[] l;
			p++;
		}

		// Texels
		else if (type.compare("vt") == 0)
		{
			char* l = new char[line.size() + 1];
			memcpy(l, line.c_str(), line.size() + 1);

			strtok(l, " ");
			for (long int i = 0; i<2; i++)
				texels[t][i] = atof(strtok(NULL, " "));

			delete[] l;
			t++;
		}

		// Normals
		else if (type.compare("vn") == 0)
		{
			char* l = new char[line.size() + 1];
			memcpy(l, line.c_str(), line.size() + 1);

			strtok(l, " ");
			for (long int i = 0; i<3; i++)
				normals[n][i] = atof(strtok(NULL, " "));

			delete[] l;
			n++;
		}

		// Faces
		else if (type.compare("f ") == 0)
		{
			char* l = new char[line.size() + 1];
			memcpy(l, line.c_str(), line.size() + 1);

			strtok(l, " ");
			for (long int i = 0; i<9; i++)
				faces[f][i] = atof(strtok(NULL, " /"));

			delete[] l;
			f++;
		}
	}

	// Close OBJ file
	inOBJ.close();
}
// 1
void writeH(string fp, string name, Model model)
{
	// 2
	// Create H file
	ofstream outH;
	outH.open(fp);
	if (!outH.good())
	{
		cout << "ERROR CREATING H FILE" << endl;
		exit(1);
	}

	// 3
	// Write to H file
	outH << "// This is a .h file for the model: " << name << endl;
	outH << endl;
	// Write statistics
	outH << "// Positions: " << model.positions << endl;
	outH << "// Texels: " << model.texels << endl;
	outH << "// Normals: " << model.normals << endl;
	outH << "// Faces: " << model.faces << endl;
	outH << "// Vertices: " << model.vertices << endl;
	outH << endl;

	// Write declarations
	outH << "const long int " << name << "Vertices;" << endl;
	outH << "const float " << name << "Positions[" << model.vertices * 3 << "];" << endl;
	outH << "const float " << name << "Texels[" << model.vertices * 2 << "];" << endl;
	outH << "const float " << name << "Normals[" << model.vertices * 3 << "];" << endl;
	outH << endl;
	// 4
	// Close H file
	outH.close();
}
void writeCvertices(string fp, string name, Model model)
{
	// Create C file
	ofstream outC;
	outC.open(fp);
	if (!outC.good())
	{
		cout << "ERROR CREATING C FILE" << endl;
		exit(1);
	}

	// Write to C file
	outC << "// This is a .c file for the model: " << name << endl;
	outC << endl;

	// Header
	outC << "#include " << "\"" << name << ".h" << "\"" << endl;
	outC << endl;

	// Vertices
	outC << "const long int " << name << "Vertices = " << model.vertices << ";" << endl;
	outC << endl;

	// Close C file
	outC.close();
}
// 1
void writeCpositions(string fp, string name, Model model, long int** faces, float** positions)
{
	// 2
	// Append C file
	ofstream outC;
	outC.open(fp, ofstream::app);

	// Positions
	outC << "const float " << name << "Positions[" << model.vertices * 3 << "] = " << endl;
	outC << "{" << endl;
	for (long int i = 0; i<model.faces; i++)
	{
		// 3
		long int vA = faces[i][0] - 1;
		long int vB = faces[i][3] - 1;
		long int vC = faces[i][6] - 1;

		// 4
		outC << positions[vA][0] << ", " << positions[vA][1] << ", " << positions[vA][2] << ", " << endl;
		outC << positions[vB][0] << ", " << positions[vB][1] << ", " << positions[vB][2] << ", " << endl;
		outC << positions[vC][0] << ", " << positions[vC][1] << ", " << positions[vC][2] << ", " << endl;
	}
	outC << "};" << endl;
	outC << endl;

	// Close C file
	outC.close();
}
void writeCtexels(string fp, string name, Model model, long int** faces, float** texels)
{
	// Append C file
	ofstream outC;
	outC.open(fp, ofstream::app);

	// Texels
	outC << "const float " << name << "Texels[" << model.vertices * 2 << "] = " << endl;
	outC << "{" << endl;
	for (long int i = 0; i<model.faces; i++)
	{
		long int vtA = faces[i][1] - 1;
		long int vtB = faces[i][4] - 1;
		long int vtC = faces[i][7] - 1;

		outC << texels[vtA][0] << ", " << texels[vtA][1] << ", " << endl;
		outC << texels[vtB][0] << ", " << texels[vtB][1] << ", " << endl;
		outC << texels[vtC][0] << ", " << texels[vtC][1] << ", " << endl;
	}
	outC << "};" << endl;
	outC << endl;

	// Close C file
	outC.close();
}

void writeCnormals(string fp, string name, Model model, long int** faces, float** normals)
{
	// Append C file
	ofstream outC;
	outC.open(fp, ofstream::app);

	// Normals
	outC << "const float " << name << "Normals[" << model.vertices * 3 << "] = " << endl;
	outC << "{" << endl;
	for (long int i = 0; i<model.faces; i++)
	{
		long int vnA = faces[i][2] - 1;
		long int vnB = faces[i][5] - 1;
		long int vnC = faces[i][8] - 1;

		outC << normals[vnA][0] << ", " << normals[vnA][1] << ", " << normals[vnA][2] << ", " << endl;
		outC << normals[vnB][0] << ", " << normals[vnB][1] << ", " << normals[vnB][2] << ", " << endl;
		outC << normals[vnC][0] << ", " << normals[vnC][1] << ", " << normals[vnC][2] << ", " << endl;
	}
	outC << "};" << endl;
	outC << endl;

	// Close C file
	outC.close();
}

int main(void) {
	// Model Info
	string obj_file_path = "model.obj";
	//string header_file1 = "human_heart_header1.h";
	string c_impl_file1 = "model.c";
	string object1_name = "model";
	Model model = getOBJinfo(obj_file_path);
	float **positions = new float*[model.positions];    // XYZ
	float **texels = new float*[model.texels];         // UV
	float **normals = new float*[model.normals];        // XYZ
	long int **faces = new long int*[model.faces];              // PTN PTN PTN
	for (long int i = 0; i < model.positions; i++) {
		positions[i] = new float[3];
	}
	for (long int i = 0; i < model.normals; i++) {
		normals[i] = new float[3];
	}
	for (long int i = 0; i < model.texels; i++) {
		texels[i] = new float[2];
	}
	for (long int i = 0; i < model.faces; i++) {
		faces[i] = new long int[9];
	}
	cout << "Model Info" << endl;
	cout << "Positions: " << model.positions << endl;
	cout << "Texels: " << model.texels << endl;
	cout << "Normals: " << model.normals << endl;
	cout << "Faces: " << model.faces << endl;
	cout << "Vertices: " << model.vertices << endl;

	extractOBJdata(obj_file_path, positions, texels, normals, faces);
	cout << "Model Data" << endl;
	cout << "P1: " << positions[0][0] << "x " << positions[0][1] << "y " << positions[0][2] << "z" << endl;
	cout << "T1: " << texels[0][0] << "u " << texels[0][1] << "v " << endl;
	cout << "N1: " << normals[0][0] << "x " << normals[0][1] << "y " << normals[0][2] << "z" << endl;
	cout << "F1v1: " << faces[0][0] << "p " << faces[0][1] << "t " << faces[0][2] << "n" << endl;

	writeH(object1_name + ".h", object1_name, model);
	// Write C file
	writeCvertices(c_impl_file1, object1_name, model);
	writeCpositions(c_impl_file1, object1_name, model, faces, positions);
	writeCtexels(c_impl_file1, object1_name, model, faces, texels);
	writeCnormals(c_impl_file1, object1_name, model, faces, normals);
	getchar();
	return 0;
}
