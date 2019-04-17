#pragma once

/**
	Renderer class - openGL is used for rendering. In this class, I've setup a very basic render loop which 
		1. Updates squares positional data based on their individual speed and velocities
		2. Checks for collisions with quadtree, and set colliding squares colors to red or back to white if they  aren't colliding anymore
		3. Renders each individual square using glDrawArrays

	Most of the logic for updating the squares is found in UpdateAndRenderSquare(), line 139

	@author: Daniel Tian - A00736794
	@date: April 9, 2019

*/


#include <fstream>
#include <algorithm>
#include <sstream>
#include  <cstdlib>

// Include GLEW. Always include it before gl.h and glfw3.h - automagical!!!
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
#include "Quadtree.h"

Quadtree* quadTree = new Quadtree(0, new Rectangle(leftBounds, bottomBounds, rightBounds, topBounds));

//100 squares
std::vector<SquareEntity> squares(300);

//opengl variables
GLuint VertexArrayID;
GLuint vertexbuffer;
GLuint colorbufferWhite;
GLuint colorbufferRed;
GLuint MatrixID;
GLuint programID;

// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 10000 units
glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 1000.0f);

// Camera matrix
glm::mat4 View = glm::lookAt(
	glm::vec3(0, 0, 250), // Camera is at (0, 0, 250), in World Space
	glm::vec3(0, 0, 0), // and looks at the origin
	glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
);

// Model matrix : an identity matrix (model will be at the origin)
glm::mat4 Model = glm::mat4(1.0f);

// Our ModelViewProjection : multiplication of our 3 matrices
glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around

static const GLfloat g_vertex_buffer_data[] = { //2 triangles -> square
	-1.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
};

static const GLfloat colorDataWhite[] = {
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
};

static const GLfloat colorDataRed[] = {
	1.0f, 0, 0,
	1.0f, 0, 0,
	1.0f, 0, 0,
	1.0f, 0, 0,
	1.0f, 0, 0,
	1.0f, 0, 0,
};

glm::vec2 getRandomDirection(glm::vec2 currentPosition) {

	float randomX = (rand() % 200) - 100;
	float randomY = (rand() % 200) - 100;

	glm::vec2 randomXY = glm::vec2(randomX, randomY);

	glm::vec2 direction = randomXY - currentPosition;

	float vectorLength = sqrt(direction.x * direction.x + direction.y * direction.y);

	glm::vec2 directionNormalized = glm::vec2(direction.x / vectorLength, direction.y / vectorLength);

	return directionNormalized;
}

void InitializeSquares() {

	//Initialize 100 squares, give them all random starting locations, speed, as well as a direction 
	float minimum_speed = 0.05f;
	float max_speed = .1f;

	//setup square entites with initial randomized data
	for (int i = 0;i < squares.size();i++) {

		squares[i].id = i;

		int r1 = rand() % (rightBounds*2); //random value between 0 and 250 or 249?
		int r2 = rand() % (topBounds*2);

		squares[i].position.x = r1 - rightBounds;	//set initial position
		squares[i].position.y = r2 - topBounds;

		squares[i].color = colorbufferWhite;	//set initial color to be white

		squares[i].direction = getRandomDirection(squares[i].position);	//sets initial velocity

		float r = minimum_speed + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max_speed - minimum_speed)));	//starting speed between low and high
		squares[i].speed = r;
	}
}

void UpdateAndRenderSquare(GLuint MatrixID, GLuint vertexbuffer, SquareEntity *square) {

	if (square->position.x > leftBounds && square->position.x < rightBounds && square->position.y > bottomBounds && square->position.y < topBounds) {
		square->position += square->direction * square->speed;	//translate the square based on randomly generated constant velocity
	}
	else {

		//generate new direction if square went out of bounds
		if (square->position.x <= leftBounds) square->position.x = leftBounds + 1;	//snap square back into our defined boundaries
		if (square->position.x >= rightBounds) square->position.x = rightBounds - 1;
		if (square->position.y <= bottomBounds) square->position.y = bottomBounds + 1;
		if (square->position.y >= topBounds) square->position.y = topBounds - 1;

		square->direction = getRandomDirection(square->position);	//generates a new random direciton based on coordinates inside the boundaries. if this causes square to move out again, we'll just try calling this again and again
	}

	Model = glm::translate(glm::mat4(1.0f), glm::vec3(square->position.x, square->position.y, 0));	//Update model's coordinates in the shader program


	
	//Retrieve a list of all other squares in the same quadrant as this square, then perform collision checks on them.
	//The returned squares are the only squares that could possibly collide with the current square. all other squares in different 
	//quadrants are ignored.
	std::vector<SquareEntity> *returnedEntities = new std::vector<SquareEntity>; //std::vector<SquareEntity> *returnedEntities = new std::vector<SquareEntity>;
		
	quadTree->Retrieve(returnedEntities, *square); //square

	bool hasCollided = false;

	for (int j = 0; j < returnedEntities->size(); j++) {//do collision checks here

		if ((*returnedEntities)[j].id == square->id) continue;

		
		float distance = sqrtf(powf(((*returnedEntities)[j].position.x - square->position.x), 2) +
			powf(((*returnedEntities)[j].position.y - square->position.y), 2));

		if (distance < 2) {
			square->color = colorbufferRed;
			hasCollided = true;
			break;
		}

	}

	
	if (!hasCollided) square->color = colorbufferWhite; 
	delete returnedEntities;
	



	//Render the square --- ModelViewProjection -> multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model; // self note: matrix multiplication is the other way around

	// pass data to shader program
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]); //"MVP" uniform

	glEnableVertexAttribArray(0); // bind vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); //1st param: match layout in shader, size, type, normalized?, stride,  buffer offset

	glEnableVertexAttribArray(1); // Bind color buffer to be that of the current square.
	glBindBuffer(GL_ARRAY_BUFFER, square->color);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


	glDrawArrays(GL_TRIANGLES, 0, 6); // draw our square (composed of 2 triangles) -  3 indices starting at 0 -> 1 triangle
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// read in vertex shader from file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Cannot open vertex shader: %s. Are you in the right directory?\n", vertex_file_path);
		getchar();
		return 0;
	}

	// read in fragment shader from file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

int InitializeOpenGL() {

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 

	// Open a window and create its OpenGL context - 1024, 768
	window = glfwCreateWindow(1000, 1000, "Assignment3-Quadtree", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE); // Capture keyboard events


	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("vertexShader.vs", "fragmentShader.fs");

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");

	//create buffers
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	
	glGenBuffers(1, &colorbufferWhite);
	glBindBuffer(GL_ARRAY_BUFFER, colorbufferWhite);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorDataWhite), colorDataWhite, GL_STATIC_DRAW);

	glGenBuffers(1, &colorbufferRed);
	glBindBuffer(GL_ARRAY_BUFFER, colorbufferRed);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorDataRed), colorDataRed, GL_STATIC_DRAW);

	glUseProgram(programID);	//use shader program - since we're using the same shader throughout this entire program, there's no need to set this more than once.

	return 0;
}

int GameLoop() {

	if (InitializeOpenGL() != 0) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	InitializeSquares();
	
	glClearColor(0,0,0, 1.0f);	//set background color to dark green 0.345f, 0.48f, 0.353f


	//main game loop
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen

		quadTree->Clear();	//every frame: clear the quadtree, then insert all squares into the quadtree
		for (int i = 0; i < squares.size(); i++) quadTree->Insert(squares[i]); 
		
		for (int i = 0; i < squares.size(); i++) UpdateAndRenderSquare(MatrixID, vertexbuffer, &squares[i]); //Render all squares, as well as update their directions if they've hit the boundaries of the screen

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
