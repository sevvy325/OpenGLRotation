#include <string>
#include <iostream>

// GLEW and Freeglut includes
#include <GL/glew.h> // This must appear before freeglut.h
#include <GL/freeglut.h>

// glm includes for matrix manipulation
#include <glm.hpp> 
#include <gtc/matrix_transform.hpp>  // Transformation related functions
#include <gtc/type_ptr.hpp> 

using namespace glm;

#define BUFFER_OFFSET(offset) ((GLvoid *) offset)

// The ID of the vertex array buffer for the cube
GLuint vertexArrayBuffers[4];

// The ID of the vertex array buffer for the triangle
GLuint triVertexArrayBufferID = 1;

// shader program ID for the cube
GLuint program;
// shader program ID for the triangle
GLuint triProgram;

// ID of the vertex attribute variable that stores the vertex position
GLuint vPos;
GLuint tVPos;

// ID of the uniform variable that stores the model-view-projection matrix
GLuint mvpMatrixID;
GLuint triMvpMatrixID;

// This array is the background to be drawn against
// Not going to use yet. Will ad backgroud image if there is Time.
//GLfloat background[][4] = {
//	{ -5.0, -5.0, -5.0, 1.0 }, // start Z+ face
//	{ 5.0, -5.0, -5.0, 1.0 },
//	{ 5.0, 5.0, -5.0, 1.0 },
//	{ -5.0, 5.0, -5.0, 1.0 },
//	{ 5.0, 5.0, -5.0, 1.0 },
//	{ -5.0, -5.0, -5.0, 1.0 } // end Z+ face
//};

// This array holds the vertices for the triangle.
GLfloat triangle[][4] = {
	{ 0.0, 1.5, 0, 1.0 },
	{ -0.75, 0.75, 0, 1.0 },
	{ 0.75, 0.75, 0, 1.0 }
};

// This array holds the indices for the triangles triangle
GLubyte triInd[] = {
	0, 1, 2
};

// This array holds the vertices for the box.
GLfloat newBox[][4] = {
	{ -0.75, -0.75, -0.75, 1.0 },
	{ -0.75, -0.75, 0.75, 1.0 },
	{ -0.75, 0.75, -0.75, 1.0 },
	{ -0.75, 0.75, 0.75, 1.0 },
	{ 0.75, -0.75, -0.75, 1.0 },
	{ 0.75, -0.75, 0.75, 1.0 },
	{ 0.75, 0.75, -0.75, 1.0 },
	{ 0.75, 0.75, 0.75, 1.0 },
	{ 0, 1.5, 0.75, 1.0}
};

// This array holds the indices of the box's trangles
GLubyte boxInd[] = {
	0, 2, 4, // -Z face
	4, 2, 6, 
	1, 3, 5, // +Z face
	5, 3, 7,
	0, 2, 1, // -X face
	1, 2, 3,
	5, 7, 4, // +X face
	4, 7, 6,
	1, 0, 5, // -Y face
	5, 0, 4,
	3, 2, 7, // +Y face
	7, 2, 6,
};


// The y rotation for the box
float boxRotaY = 0.0;
// The y rotation for the triangle
float triRotaZ = 0.0;


mat4 projMatrix;
mat4 viewMatrix;
mat4 modelMatrix;

// Initialize data arrays and shaders
void init()
{
	// The vertex coordinates are hard coded in the program for simplicity. 
	// Normally they are loaded from a 3D model file. 

	// Get an unused buffer object name. Required after OpenGL 3.1. 
	glGenBuffers(4, vertexArrayBuffers);

	// If it's the first time the buffer object name is used, create that buffer. 
	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[0]);

	// Allocate memory for the active buffer object. 
	// 1. Allocate memory on the graphics card for the amount specified by the 2nd parameter.
	// 2. Copy the data referenced by the third parameter (a pointer) from the main memory to the 
	//    memory on the graphics card. 
	// 3. If you want to dynamically load the data, then set the third parameter to NULL. 
	// 
	// sizeof(vertices) only works for statically allocated arrays (like here). 
	// For dynamically allocated arrays, sizeof(vertices) returns 4 -- the number of bytes for a pointer. 
	// For dynamically allocated arrays, you need to calculate the number of bytes and specify them here. 
	glBufferData(GL_ARRAY_BUFFER, sizeof(newBox), newBox, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxInd), boxInd, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triInd), triInd, GL_STATIC_DRAW);

	// OpenGL vertex shader source code
	const char* vSource = {
		"#version 330\n"
		"in vec4 vPos;"
		"uniform mat4 mvp_matrix;"
		"void main() {"
		"	gl_Position = mvp_matrix * vPos;"
		"}"
	};

	// OpenGL fragment shader source code
	const char* fSource = {
		"#version 330\n"
		"out vec4 fragColor;"
		"void main() {"
		"	fragColor = vec4(0.7, 0.7, 1.0, 1.0);"
		"}"
	};

	// OpenGL vertex shader source code
	const char* tVSource = {
		"#version 330\n"
		"in vec4 vPos;"
		"uniform mat4 mvp_matrix;"
		"void main() {"
		"	gl_Position = mvp_matrix * vPos;"
		"}"
	};

	// OpenGL fragment shader source code
	const char* tFSource = {
		"#version 330\n"
		"out vec4 fragColor;"
		"void main() {"
		"	fragColor = vec4(0.5, 0.0, 0.0, 1.0);"
		"}"
	};

	// Declare shader IDs
	GLuint vShader, fShader, tVShader, tFShader;

	// Create empty shader objects
	vShader = glCreateShader(GL_VERTEX_SHADER);
	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	tVShader = glCreateShader(GL_VERTEX_SHADER);
	tFShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Attach shader source code the shader objects
	glShaderSource(vShader, 1, &vSource, NULL);
	glShaderSource(fShader, 1, &fSource, NULL);
	glShaderSource(tVShader, 1, &tVSource, NULL);
	glShaderSource(tFShader, 1, &tFSource, NULL);

	// Compile shader objects
	glCompileShader(vShader);
	glCompileShader(fShader);
	glCompileShader(tVShader);
	glCompileShader(tFShader);

	// Create an empty shader program object
	program = glCreateProgram();

	// Attach vertex and fragment shaders to the shader program
	glAttachShader(program, vShader);
	glAttachShader(program, fShader);

	// Link the shader program
	glLinkProgram(program);

	triProgram = glCreateProgram();

	glAttachShader(triProgram, tVShader);
	glAttachShader(triProgram, tFShader);

	glLinkProgram(triProgram);




	// Retrieve the ID of a vertex attribute variable and the uniform matrix variable. 
	vPos = glGetAttribLocation(program, "vPos");
	tVPos = glGetAttribLocation(triProgram, "vPos");
	mvpMatrixID = glGetUniformLocation(program, "mvp_matrix");
	triMvpMatrixID = glGetUniformLocation(triProgram, "mvp_matrix");

	// Turn on depth test
	glEnable(GL_DEPTH_TEST);

	// Specify the background color
	glClearColor(0.025f, 0.f, 0.1f, 1);	// 176 - 224 - 230
}

// Handles the reshape event
void reshape(int width, int height)
{
	// Specify the width and height of the picture within the window
	// Creates a viewport matrix and insert it into the graphics pipeline. 
	// This operation is not done in shader, but taken care of by the hardware. 
	glViewport(0, 0, width, height);

	// Creates a projection matrix. 
	// This is a glm function. 
	// The first parameter is the camera field of view in radians. 
	// Note that some glm functions such as perspective() and rotate() use radians (instead of degrees) by default. 
	// You can convert degrees to radians via glm function radians(). For a complete list of glm functions that requires
	// radians, check out the latest glm release notes at http://glm.g-truc.net/0.9.8/updates.html.

	// The second parameter is the window aspect ratio.
	// The third parameter is the distance of the near clipping plane to the camera. 
	// The fourth parameter is the distance of the far clipping plane.
	// Experiment with different field of view and see the difference. 
	projMatrix = perspective(radians(60.0f), (float)width / (float)height, 0.1f, 5000.0f);

	// This is a glm function. 
	// lookAt() creates a view matrix based on the camera location and orientation.  
	// The first parameter is the location of the camera; 
	// the second is where the camera is pointing at; the third is the up vector for camera.
	// If you need to move or animate your camera during run time, then you need to construct the 
	// view matrix in display() function. 
	// Experiment with different camera location and orientation to see the differences. 
	viewMatrix = lookAt(vec3(0.0f, 0.0f, 3.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
}

// Handles the display event
void display()
{
	// Clear the window with the background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Activate the shader program
	glUseProgram(program);

	// Create individual translation/rotation/scaling matrices. You transform a 3D object to a 
	// specific location by a sequence of translations, rotations, and scalings. Each transformation
	// is encoded in an elementary transformation matrix. 
	// Experiment with different transformation parameters and see the difference. 
	mat4 scaleMatrix = scale(mat4(1.0f), vec3(0.8f));

	mat4 translateMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));

	mat4 rotationMatrixX = rotate(mat4(1.0f), radians(45.0f), vec3(1.0f, 0.0f, 0.0f));
	mat4 rotationMatrixY = rotate(mat4(1.0f), radians(boxRotaY), vec3(0.0f, 1.0f, 0.0f));
	mat4 rotationMatrixZ = rotate(mat4(1.0f), radians(0.0f), vec3(0.0f, 0.0f, 1.0f));

	// The overall transformation matrix is created by multiplying these elementary matrices. 
	// This is the model matrix. 
	// 
	// It's important to know how to interpret the sequence of transformations. 
	// Note that in the vertex shader, the vertex position is multiplied from the right side of the matrix. 
	// Therefore, to read the sequence of transformations, start from the right side and read from right to left. 
	// In this case, each vertex is first scaled, then rotated, and then translated.
	// The sequence of the matrix multiplication matters. You may change the sequence and see the differences. 
	modelMatrix = translateMatrix * rotationMatrixZ * rotationMatrixY * rotationMatrixX * scaleMatrix;

	// Model matrix is then multiplied with view matrix and projection matrix to create a combined
	// model_view_projection matrix. 
	// The view and project matrix are created in reshape() callback function. However, if you need
	// to move the camera or change focal length (Fielf of View) during run time, then you need to create
	// projection and view matrix in the display() callback function. Why? Because display() function
	// is called more frequently than reshape() (which is only called when the window is created or resized). 
	// Therefore any change you make to the view and projection matrix can be instantly displayed. 
	//
	// The sequence of multiplication is important here. Model matrix, view matrix, and projection matrix 
	// must be multiplied from right to left, because in vertex shader the vertex position 
	// is multiplied from the right hand side. 
	mat4 mvpMatrix = projMatrix * viewMatrix * modelMatrix;

	// The model_view_projection matrix is transferred to the graphics memory to be used in the vertex shader. 
	// Here we send the combined model_view_projection matrix to the shader so that we don't have to do the same 
	// multiplication in the vertex shader repeatedly. Note that the vertex shader is executed for each vertex. 
	// Sometimes you need to send individual model, view, or projection matrices to the shader.
	// The vertex shader may need these matrices for other calculations. 
	// 
	// It's important to understand how the parameters in the shader and the parameters in this (OpenGL) program are
	// connected. In this case, how the matrices are connected. If you makes changes in the shader, make sure you update
	// the corresponding parameters in the OpenGL program, and vice versa. 
	glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, value_ptr(mvpMatrix));

	// If the buffer object already exists, make that buffer the active one. 
	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[0]);

	// Associate the vertex array in the buffer object with the vertex attribute: "position".
	// Note this is how the shader program object and (vertex array) buffer object is connected. 
	// vPos is an ID associated with a vertex attribute in the shader program. Now vPos is 
	// associated with a vertex array buffer object. So when the shader program is executed,
	// it finds a vertex array through vPos. 
	glVertexAttribPointer(vPos, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// Enable the vertex attribute: "position" so that when the shader program executes,
	// it will look for a vertex array through vPos. If other vertex attributes are 
	// enabled, the shader program will also look for (e.g. color, normal) arrays from their 
	// corresponding IDs. 
	glEnableVertexAttribArray(vPos);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexArrayBuffers[1]);

	// Start the shader program
	//glDrawArrays(GL_TRIANGLES, 0, sizeof(box));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
	
	glUseProgram(triProgram);

	scaleMatrix = scale(mat4(1.0f), vec3(0.8f));

	translateMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));

	rotationMatrixX = rotate(mat4(1.0f), radians(45.0f), vec3(1.0f, 0.0f, 0.0f));
	rotationMatrixY = rotate(mat4(1.0f), radians(boxRotaY), vec3(0.0f, 1.0f, 0.0f));
	rotationMatrixZ = rotate(mat4(1.0f), radians(triRotaZ), vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = translateMatrix * rotationMatrixY * rotationMatrixX * rotationMatrixZ * scaleMatrix;
	mvpMatrix = projMatrix * viewMatrix * modelMatrix;

	glUniformMatrix4fv(triMvpMatrixID, 1, GL_FALSE, value_ptr(mvpMatrix));

	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[2]);
	glVertexAttribPointer(tVPos, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(tVPos);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexArrayBuffers[3]);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));

	// If the buffer ID is 0, disable the buffer.  
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Swap the front and back buffers to refresh the window
	glutSwapBuffers();
}

void update(int value) {
	// increase the rotation for the box
	boxRotaY += 1.0f;

	//increase the rotation for the triangle
	triRotaZ += 1.0f;

	glutTimerFunc(10, update, 1);

	// update the display
	glutPostRedisplay();
}

void main(int argc, char *argv[])
{
	// Initialize Freeglut
	glutInit(&argc, argv);

	// Initialize window display mode. 
	// Note that the depth buffer is initialized here. 
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutCreateWindow("CUBEZ");

	glutReshapeWindow(500, 400);

	// Initialize Glew
	glewInit();

	// Prepare and transfer 3D data and shaders from CPU to GPU.
	init();

	// Register the display callback function
	glutDisplayFunc(display);

	// Register the reshape callback function
	glutReshapeFunc(reshape);

	// Update every frame
	glutTimerFunc(10, update, 1);

	// Start the main event loop
	glutMainLoop();
}