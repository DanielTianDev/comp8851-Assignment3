#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>

#include <fstream>
#include <algorithm>
#include <sstream>
#include  <cstdlib>


// Include GLEW. Always include it before gl.h and glfw3.h, since it's a bit magic.
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;


struct squareEntity {
	glm::vec2 position;
	GLuint color;
	glm::vec2 direction;
	float speed = 0.0f;
};

GLfloat leftBounds = -125,
rightBounds = 125,
topBounds = 100,
bottomBounds = -100;

glm::vec2 getRandomDirection(glm::vec2 currentPosition) {

	float randomX = (rand() % 250) - 125;
	float randomY = (rand() % 200) - 100;

	glm::vec2 randomXY = glm::vec2(randomX, randomY);

	glm::vec2 direction = randomXY - currentPosition;

	float vectorLength = sqrt(direction.x * direction.x + direction.y * direction.y);

	glm::vec2 directionNormalized = glm::vec2(direction.x / vectorLength, direction.y / vectorLength);

	return directionNormalized;
}


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


GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
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




int RenderLoop() {


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
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Tutorial 03 - Matrices", NULL, NULL);
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

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("vertexShader.vs", "fragmentShader.fs");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 10000 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 10000.0f);
	// Or, for an ortho camera :
	//glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

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




	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	//GLuint colorbuffer;
	//glGenBuffers(1, &colorbuffer);
	//glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	GLuint colorbufferWhite;
	glGenBuffers(1, &colorbufferWhite);
	glBindBuffer(GL_ARRAY_BUFFER, colorbufferWhite);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorDataWhite), colorDataWhite, GL_STATIC_DRAW);

	GLuint colorbufferRed;
	glGenBuffers(1, &colorbufferRed);
	glBindBuffer(GL_ARRAY_BUFFER, colorbufferRed);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorDataRed), colorDataRed, GL_STATIC_DRAW);

	std::vector<squareEntity> squares(100);
	
	//setup square entites with initial randomized data
	for (int i = 0;i < squares.size();i++) {

		int r1 = rand() % 250; //random val between 0 and 100
		int r2 = rand() % 200;

		squares[i].position.x = r1 - 125;
		squares[i].position.y = r2 - 100;

		squares[i].color = colorbufferWhite;

		squares[i].direction = getRandomDirection(squares[i].position);

		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		squares[i].speed = r;
	}

	squares[0].color = colorbufferRed;

	do {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);



		for (int i = 0; i < squares.size(); i++) {

			if (squares[i].position.x > leftBounds && squares[i].position.x < rightBounds
				&& squares[i].position.y > bottomBounds && squares[i].position.y < topBounds) {

				squares[i].position += squares[i].direction * squares[i].speed;
			}
			else {

				//generate new direction
				if (squares[i].position.x <= leftBounds) squares[i].position.x = leftBounds + 1;
				if (squares[i].position.x >= rightBounds) squares[i].position.x = rightBounds - 1;
				if (squares[i].position.y <= bottomBounds) squares[i].position.y = bottomBounds + 1;
				if (squares[i].position.y >= topBounds) squares[i].position.y = topBounds - 1;

				squares[i].direction = getRandomDirection(squares[i].position);
			}

			Model = glm::translate(glm::mat4(1.0f), glm::vec3(squares[i].position.x, squares[i].position.y, 0));

			// Our ModelViewProjection : multiplication of our 3 matrices
			glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around



			// pass data to shader program
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]); //"MVP" uniform

			glEnableVertexAttribArray(0); // 1st attribute buffer : vertices
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glVertexAttribPointer(
				0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);
			

			glEnableVertexAttribArray(1); // 2nd attribute buffer : colors
			glBindBuffer(GL_ARRAY_BUFFER, squares[i].color);
			glVertexAttribPointer(
				1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
				3,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			// draw our square (composed of 2 triangles)
			glDrawArrays(GL_TRIANGLES, 0, 6); // 3 indices starting at 0 -> 1 triangle

			glDisableVertexAttribArray(0);

		}


		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} 
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0); // quit when escape key pressed, or window is closed


	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();


}



