/*
 * Skeleton code for AAA633 Visual Computing
 *
 * Won-Ki Jeong, wkjeong@kroea.ac.kr
 *
 */
#define ARGUMENT	0

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <assert.h>
#include "textfile.h"

 //my functions and parameters

struct Coordinate {
	float x;
	float y;
	float z;
};

int NumOfVertex = 0;
int NumOfIndex = 0;
/*
float* vertices;
int* indices;
float* vertexnormals;
float* facenormals;
float* colors;
*/

std::vector<float> vertices;
std::vector<uint32_t> indices;
std::vector<float> vertexnormals;
std::vector<float> facenormals;
std::vector<float> colors;

std::vector<float> testvert{ 0, 0, 0,
							-0.5, 0.5, 0.0,
							0.5, 0.5, 0,
							1,0,0,
							0.5, 0.5, 0,
							-0.5, -0.5, 0,
							-1, 0, 0 };

GLuint VAO;
GLuint VBO;
GLuint IBO;
GLuint CBO;//color
GLuint NBO;//normal


void LoadData(std::string FilePath);
void GetNormals();
void FaceNormal();
void Colors();
Coordinate CalFaceNormal(Coordinate v1, Coordinate v2, Coordinate v3);


// Shader programs
GLuint p;

// Light position
float lpos[4] = { 1,0.5,1,0 };

void reshape(int w, int h)
{
	glLoadIdentity();
	glViewport(0, 0, w, h);
	/*glOrtho(0, 100, 0, 100, 100, -100);*/
	glOrtho(-1.0, 1.0, -1.0, 1.0, 10, -10);
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == 'p' || key == 'P') {
		std::cout << "press P! \r\n";
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0, 100, 0, 0, 0, 0, -1, 0, 0);
		glLoadIdentity();
		glFrustum(-1, 1, -1, 1, 1, 10);
	}
	else if (key == 'o' || key == 'O') {
		std::cout << "press o! \r\n";
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0, 100, 0, 0, 0, 0, -1, 0, 0);
		glLoadIdentity();
		glOrtho(-100, 100, -100, 100, 0, 200);
	}

	glutPostRedisplay();
}

void renderScene(void)
{
	//std::cout << " render \r\n";
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);


	glutSwapBuffers();

}

void idle()
{
	glutPostRedisplay();
}

//
// Main
//
int main(int argc, char** argv)
{
	//0. parameters
	std::string FilePath;

#if ARGUMNET
	//1. command parsing
	for (int i = 1; i < argc - 1; i++) {

		if (!strcmp(argv[i], "-f")) {
			if (argc < i + 1) {
				std::cout << "-f \"File name\"" << std::endl;
				return -1;
			}
			else {
				if (!FilePath.size()) {
					i++;
					FilePath = argv[i];
					if (FilePath.rfind(".off") == -1) {
						std::cout << "File name error : must be contained '.off'" << std::endl;
						return -1;
					}
				}
				else {
					std::cout << "FilePath string already get the File name" << std::endl;
					std::cout << FilePath.size() << std::endl;
					std::cout << FilePath << std::endl;

					return -1;
				}

			}
		}
	}
#else
	//FilePath = "../../mesh-data/bunny.off";
	FilePath = "../mesh-data/bunny.off";
	//FilePath = "../mesh-data/fandisk.off";

	LoadData(FilePath);
	FaceNormal();
	GetNormals();
#endif

#if 0
	for (int i = 0; i < NumOfVertex * 3; i++) {
		std::cout << vertices[i] << " ";
		if (i % 3 == 2) {
			std::cout << std::endl;
		}
	}
	for (int i = 0; i < NumOfIndex * 3; i++) {
		std::cout << indices[i] << " ";
		if (i % 3 == 2) {
			std::cout << std::endl;
		}
	}
#endif


	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("AAA633 - Assignment 1");

	// register callbacks
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	//glutReshapeFunc(changeSize);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	glewInit();
	if (glewIsSupported("GL_VERSION_3_3")) {
		printf("Ready for OpenGL 3.3\n");
	}
	else {
		printf("OpenGL 3.3 is not supported\n");
		exit(1);
	}

	// Create shader program
	p = createGLSLProgram("../phong.vert", NULL, "../phong.frag"); // Phong

	/*VBO, VAO, IBO / buffer init*/
	//glGenVertexArrays(1, &VAO);
	//glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	/*glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);*/
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* vertices.size(), &vertices[0], GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glGenBuffers(1, &NBO);
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* vertexnormals.size(), &vertexnormals[0], GL_STATIC_DRAW);
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, 0, 0);

#if 0
	glGenBuffers(1, &CBO);
	glBindBuffer(GL_ARRAY_BUFFER, CBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * colors.size(), colors.data(), GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &NBO);
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexnormals.size(), vertexnormals.data(), GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), indices.data(), GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

#if 0
	/*attr set*/
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, vertices.size(), GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
#endif

	/*element set*/
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	/*glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);*/
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t)* indices.size(), &indices[0], GL_STATIC_DRAW);
	glEnableClientState(GL_INDEX_ARRAY);
	glIndexPointer(GL_UNSIGNED_INT, 0, 0);

	//std::cout << vertices.size() << " " << vertices[NumOfVertex * 3 - 1] << std::endl;

	//Ray
	
	GLfloat lightPos1[] = { 10, 10, -3, 1 };
	GLfloat diffuse1[] = { 0.5, 0.5, 0.0, 1 };
	GLfloat specular1[] = { 0.5, 0.5, 0.5, 1 };
	GLfloat ambient1[] = { 0.1, 0.1, 0.1, 1 };

	GLfloat lightPos2[] = { -10, 10, 1, 1 };
	GLfloat diffuse2[] = { 0.0, 0.9, 0.9, 1 };
	GLfloat specular2[] = { 0, 0, 1, 1 };
	GLfloat ambient2[] = { 0.5, 0.5, 0.5, 1 };

	GLfloat lightPos3[] = { 0, 3, 5, 1 };
	GLfloat diffuse3[] = { 0.0, 0.9, 0.9, 1 };
	GLfloat specular3[] = { 1, 1, 1, 1 };
	GLfloat ambient3[] = { 1, 1, 1, 1 };
	


	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}

void LoadData(std::string FilePath) {

	std::ifstream OpenFile(FilePath);

	if (!OpenFile.is_open()) {
		std::cout << FilePath << " file can't open" << std::endl;
		return;
	}

	std::string line;
	std::string str;

	//0th line = "OFF"
	getline(OpenFile, line);
	if (line != "OFF") {
		std::cout << "Wrong Header!" << std::endl;
		return;
	}

	//1th line = #vertices #indices
	getline(OpenFile, line);
	std::istringstream iss(line);

	int i = 0;

	while (getline(iss, str, ' ')) {

		std::stringstream ssint(str);
		if (i == 0) {
			ssint >> NumOfVertex;
		}
		else if (i == 1) {
			ssint >> NumOfIndex;
		}
		i++;
	}

	//	vertices = (float*)malloc(sizeof(float) * 3 * NumOfVertex);
	//	indices = (int*)malloc(sizeof(int) * 3 * NumOfIndex);

	for (i = 0; i < NumOfVertex; i++) {
		getline(OpenFile, line);
		std::istringstream coord(line);
		std::string buf;
		int j = 0;
		while (getline(coord, buf, ' ')) {
			float tempfloat;
			std::stringstream dat(buf);
			dat >> tempfloat;
			vertices.push_back(tempfloat);
			j++;
		}
	}

	for (i = 0; i < NumOfIndex; i++) {
		getline(OpenFile, line);
		std::istringstream coord(line);
		std::string buf;
		int j = 0;
		while (getline(coord, buf, ' ')) {
			int tempint;
			std::stringstream dat(buf);
			if (j % 4 != 0) {
				dat >> tempint;
				indices.push_back(tempint);
			}
			j++;
		}
	}
}

void GetNormals() {

	vertexnormals.assign(NumOfVertex * 3, 0);

	for (int i = 0; i < indices.size() / 3; i++) {
		vertexnormals[indices[i] * 3] += facenormals[i * 3];
		vertexnormals[indices[i] * 3 + 1] += facenormals[i * 3 + 1];
		vertexnormals[indices[i] * 3 + 2] += facenormals[i * 3 + 2];
	}

	for (int i = 0; i < vertexnormals.size(); i++) {
		vertexnormals[i] = vertexnormals[i] / count(indices.begin(), indices.end(), i / 3);
	}

	return;
}

void FaceNormal() {

	for (int i = 0; i < NumOfIndex; i++) {
		Coordinate v1 = { vertices[indices[i * 3]], vertices[indices[i * 3] + 1], vertices[indices[i * 3] + 2] };
		Coordinate v2 = { vertices[indices[i * 3 + 1]], vertices[indices[i * 3 + 1] + 1], vertices[indices[i * 3 + 1] + 2] };
		Coordinate v3 = { vertices[indices[i * 3 + 2]], vertices[indices[i * 3 + 2] + 1], vertices[indices[i * 3 + 2] + 2] };
		Coordinate res = CalFaceNormal(v1, v2, v3);
		facenormals.push_back(res.x);
		facenormals.push_back(res.y);
		facenormals.push_back(res.z);
	}

}
void Colors() {

	for (int i = 0; i < NumOfVertex * 3; i++) {
		if (i % 3)
			colors.push_back(200.0 / 255.0);
	}
}
Coordinate CalFaceNormal(Coordinate v1, Coordinate v2, Coordinate v3) {
	Coordinate normal;
	float normalsize;


	normal.x = v1.y * v2.z - v1.z * v2.y;
	normal.y = -(v1.x * v2.z - v1.z * v2.x);
	normal.z = v1.x * v2.y - v1.y * v2.x;

	normalsize = normal.x * normal.x + normal.y * normal.y + normal.z * normal.z;
	normalsize = sqrt(normalsize);

	normal.x = normal.x / normalsize;
	normal.y = normal.y / normalsize;
	normal.z = normal.z / normalsize;

	return normal;
}