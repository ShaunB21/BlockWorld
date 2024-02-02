// Code adapted from www.learnopengl.com, www.glfw.org

#include <iostream>
#include "vertices.h"
#include "noise.h"
#include "chunk.h"
#include "chunkGenerator.h"
#include <fstream>
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>


// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "shader.h"

ChunkGenerator chunkGen;

bool flightMode = true;
const int seed = 333;
// Method definitions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void do_movement();
void generateChunks();

// Window dimensions
const GLuint winWidth = 1920, winHeight = 1080;

// How many blocks from the camera position are rendered
// MUST BE AN EVEN NUMBER
const int renderDistance = 20;
float colour; 
Chunk chunks[renderDistance][renderDistance];
// sets camera position to be the middle of the world
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// sets light position to be the middle of the world
glm::vec3 lightPos(0.0f, 100.0f, 0.0f);

// Mouse movement
GLfloat yaw = -90.0f;	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
GLfloat pitch = 0.0f;
GLfloat lastX = winWidth / 2.0;
GLfloat lastY = winHeight / 2.0;

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

GLint objectColorLoc;
GLint modelLoc;

unsigned int VBO, VAO;

int currentChunkX;
int currentChunkZ;
int lastChunkX;
int lastChunkZ;

bool keys[1024];

// Defines function for handling keypresses
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		flightMode = !flightMode;
	}
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

// Defines function for handling camera movement keypresses and mouse inputs
void do_movement()
{
	if (flightMode == false) {
		// Camera controls
		GLfloat cameraSpeed = 10.0f * deltaTime;
		if (keys[GLFW_KEY_W])
			cameraPos += cameraSpeed * cameraFront;
		if (keys[GLFW_KEY_S])
			cameraPos -= cameraSpeed * cameraFront;
		if (keys[GLFW_KEY_A])
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (keys[GLFW_KEY_D])
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (keys[GLFW_KEY_SPACE])
			cameraPos += cameraSpeed * cameraUp;
	}
	else if (flightMode == true) {
		GLfloat cameraSpeed = 40.0f * deltaTime;
		if (keys[GLFW_KEY_W])
			cameraPos += cameraSpeed * cameraFront;
		if (keys[GLFW_KEY_S])
			cameraPos -= cameraSpeed * cameraFront;
		if (keys[GLFW_KEY_A])
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (keys[GLFW_KEY_D])
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (keys[GLFW_KEY_SPACE])
			cameraPos += cameraSpeed * cameraUp;
		if (keys[GLFW_KEY_LEFT_SHIFT])
			cameraPos -= cameraSpeed * cameraUp;
	}



}

bool firstMouse = true;

// Defines function for handling mouse input
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (GLfloat)xpos;
		lastY = (GLfloat)ypos;
		firstMouse = false;
	}

	GLfloat xoffset = (GLfloat)xpos - lastX;
	GLfloat yoffset = lastY - (GLfloat)ypos; // Reversed since y-coordinates go from bottom to left
	lastX = (GLfloat)xpos;
	lastY = (GLfloat)ypos;

	GLfloat sensitivity = (GLfloat)0.05;	// Change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

// The MAIN function, from here we start the application and run the game loop
int main(void)
{
	//++++create a glfw window+++++++++++++++++++++++++++++++++++++++
	GLFWwindow* window;

	if (!glfwInit()) //Initialize the library
		return -1;

	// glfwGetPrimaryMonitor()
	window = glfwCreateWindow(winWidth, winHeight, "OpenGL Window", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(window);//Make the window's context current
	//++++Initialize GLEW to setup the OpenGL Function pointers+++++++
	glewExperimental = GL_TRUE;
	glewInit();

	//++++Define the viewport dimensions++++++++++++++++++++++++++++
	glViewport(0, 0, winWidth, winHeight);
	// Setup OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);

	// First, set the container's VAO (and VBO)
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Creates a VAO and VBO 

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubesArray), cubesArray, GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Build and compile our shader program
	GLuint shaderProgram = initShader("vert.glsl", "frag.glsl");

	// Use cooresponding shader when setting uniforms/drawing objects
	glUseProgram(shaderProgram);
	objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
	GLint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
	GLint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
	GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");

	// Get the uniform locations
	modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
	
	glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
	glm::mat4 projection;
	projection = glm::perspective(45.0f, (GLfloat)winWidth / (GLfloat)winHeight, 0.1f, 1000.0f);

	lastChunkX = cameraPos.x / chunkWidth;
	lastChunkZ = cameraPos.z / chunkWidth;
	bool firstRender = true;
	double lastTime = glfwGetTime();
	int nbFrames = 0;
	// Game loop
	while (!glfwWindowShouldClose(window))
	{


		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
			// printf and reset timer
			printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}
		
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		do_movement();

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Clear the colorbuffer
		glClearColor(99.0f / 255.0f, 180.0f / 255.0f, 255.0f / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);

		// Create camera transformations
		glm::mat4 view;
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		// Draw the objects

		// Gets chunk that camera position is in
		currentChunkX = cameraPos.x / chunkWidth;
		currentChunkZ = cameraPos.z / chunkWidth;
		

		if (lastChunkX != currentChunkX || lastChunkZ != currentChunkZ || firstRender == true) {
			generateChunks();
		}

		// Loops through the array of chunks and every block within the chunks and creates a cube at each position
		for (int i = 0; i < renderDistance; i++) {
			for (int j = 0; j < renderDistance; j++) {
				Chunk chunk = chunks[i][j];
				for (int m = 0; m < chunkDepth; m++) {
					for (int n = 0; n < chunkWidth; n++) {
						for (int o = 0; o < chunkWidth; o++) {
							glm::mat4 model;
							// Set colour of cubes to green
							glUniform3f(objectColorLoc, 0.0f / 255.0f, 150.0f / 255.0f, 0.0f / 255.0f);
							// Set colour of cubes below specified height to sand
							if (chunk.blockPositions[m][n][o].y < -15.0f) {
								glUniform3f(objectColorLoc, 186.0f / 255.0f, 170.0f / 255.0f, 155.0f / 255.0f);
							}
							// Set colour of cubes below specified height to dark green
							if (chunk.blockPositions[m][n][o].y > 0.0f) {
								glUniform3f(objectColorLoc, 40.0f / 255.0f, 150.0f / 255.0f, 26.0f / 255.0f);
							}
							// Generates cube for every block in every chunk
							model = glm::translate(model, glm::vec3(chunk.blockPositions[m][n][o]));
							glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
							glBindVertexArray(VAO);
							// Draw shape
							glDrawArrays(GL_TRIANGLES, 0, 36);
						}

					}
				}

			}
		}
		
		lastChunkX = cameraPos.x / chunkWidth;
		lastChunkZ = cameraPos.z / chunkWidth;
		firstRender = false;
		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

void generateChunks() {
	int x = 0;
	for (int i = (-renderDistance / 2) + currentChunkX; i < (renderDistance / 2) + currentChunkX; i++) {
		int y = 0;
		for (int j = (-renderDistance / 2) + currentChunkZ; j < (renderDistance / 2) + currentChunkZ; j++) {
			Chunk chunk = chunkGen.generateChunk(seed, i, j);
			chunks[x][y] = chunk;
			y++;
		}
		x++;
	}
}

