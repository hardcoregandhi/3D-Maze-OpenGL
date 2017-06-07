#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <gl\glut.h>
#include <gl\freeglut.h>
#include <GL\freeglut.h>
#include <GL/freeglut_ext.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "Shader.h"
#include "GroundPlane.h"
#include "Player.h"

using namespace glm;
GLFWwindow* window;

int MAZE_BOUNDS = 20;
vec2 playerStartingPosition;
vec2 goalPosition;
int maze [20][20] =
{ 
{ 1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,9,1,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0,1 },
{ 1,0,1,0,0,0,1,0,1,1,1,0,1,1,0,1,0,1,0,1 },
{ 1,0,0,0,1,1,1,0,0,0,0,0,1,1,0,1,1,0,0,1 },
{ 1,0,1,0,0,0,0,0,1,1,1,0,0,1,0,0,0,0,1,1 },
{ 1,0,1,0,1,1,1,0,1,0,0,0,1,1,0,1,1,1,0,1 },
{ 1,0,1,0,1,0,0,0,1,1,1,0,1,1,0,1,1,1,0,1 },
{ 1,0,1,0,1,1,1,0,1,0,1,0,1,1,0,0,0,0,0,1 },
{ 1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,1,0,1,0,1 },
{ 1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,1,0,1,0,1 },
{ 1,0,1,0,0,0,0,0,1,0,1,0,1,0,0,0,1,1,0,1 },
{ 1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,1,0,1 },
{ 1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,0,1,0,1 },
{ 1,0,1,1,1,1,1,0,1,0,1,0,1,0,1,1,0,1,0,1 },
{ 1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,0,1,0,1 },
{ 1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,1,0,1 },
{ 1,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,1,0,1 },
{ 1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,0,0,0,1 },
{ 1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1,1,0,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,5,1 },
};

#include "Controls.h" //controls needs maze

int main()
{
	/////////////////////////////////////////////////////////////////////
	// Window Creation
	/////////////////////////////////////////////////////////////////////

	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}
	int myargc = 1;
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "OpenGL GLFW Window", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	//background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	/////////////////////////////////////////////////////////////////////
	//	Shaders
	/////////////////////////////////////////////////////////////////////
	GLuint colourShaderID = LoadShaders("VertexShader.vertexshader", "FragmentShader.fragmentshader");
	GLuint colourShaderModelID = glGetUniformLocation(colourShaderID, "model");
	GLuint colourShaderViewID = glGetUniformLocation(colourShaderID, "view");
	GLuint colourShaderProjectionID = glGetUniformLocation(colourShaderID, "projection");
	GLuint colourShaderColourID = glGetUniformLocation(colourShaderID, "userColour");
	glUniform3f(colourShaderColourID, 0.0f, 1.0f, 0.0f);

	/////////////////////////////////////////////////////////////////////
	//	Initialisation
	/////////////////////////////////////////////////////////////////////

	//dynamic settings to allow for map changes
	for (int i = 0; i < MAZE_BOUNDS; i++)
	{
		for (int j = 0; j < MAZE_BOUNDS; j++)
		{
			if (maze[i][j] == 5)
				playerStartingPosition = vec2(i, j);
			else if (maze[i][j] == 9)
				goalPosition = vec2(i, j);
		}
	}

	vec3 mazeColour = vec3(0.0f, 1.0f, 1.0f);
	vec3 playerColour = vec3(1.0f, 0.0f, 0.0f);
	vec3 goalColour = vec3(0.0f, 1.0f, 0.0f);

	GroundPlane* groundPlane = new GroundPlane(colourShaderColourID);
	Player* player = new Player(colourShaderColourID);

	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//INPUT/UPDATE
		computeMatricesFromInputs(window);

		glUseProgram(colourShaderID);
		glUniform3f(colourShaderColourID, 0.0f, 1.0f, 0.0f);

		/////////////////////////////////////////////////////////////////////
		//	Camera
		/////////////////////////////////////////////////////////////////////
		ProjectionMatrix = getProjectionMatrix();
		ViewMatrix = getViewMatrix();
		glUniformMatrix4fv(colourShaderProjectionID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
		glUniformMatrix4fv(colourShaderViewID, 1, GL_FALSE, &ViewMatrix[0][0]);
		glm::mat4 model = glm::mat4(1.0);
		/////////////////////////////////////////////////////////////////////
		//	Draw
		/////////////////////////////////////////////////////////////////////
		for (int i = 0; i < MAZE_BOUNDS; i++)
		{
			for (int j = 0; j < MAZE_BOUNDS; j++)
			{
				if (maze[i][j] == 1) //maze walls
				{
					glm::mat4 model = glm::mat4(1.0);
					model = glm::scale(model, vec3(0.5, 0.5, 0.5));
					model = glm::translate(model, vec3(i*2, 0, j*2));

					glUniformMatrix4fv(colourShaderModelID, 1, GL_FALSE, &model[0][0]);
					groundPlane->Draw(vec3(i*10, 0, j*10), colourShaderColourID, mazeColour);
				}
				else if (maze[i][j] == 5) //player
				{
					glm::mat4 model = glm::mat4(1.0);
					model = glm::scale(model, vec3(0.5, 0.5, 0.5));
					model = glm::translate(model, vec3(i * 2, 0, j * 2));
					glUniform3f(colourShaderColourID, playerColour.r, playerColour.g, playerColour.b);

					glUniformMatrix4fv(colourShaderModelID, 1, GL_FALSE, &model[0][0]);
					player->Draw();

				}
				else if (maze[i][j] == 9) //goal
				{
					glm::mat4 model = glm::mat4(1.0);
					model = glm::scale(model, vec3(0.5, 0.5, 0.5));
					model = glm::translate(model, vec3(i * 2, 0, j * 2));
					glUniform3f(colourShaderColourID, 1.0f, 0.0f, 0.0f);

					glUniformMatrix4fv(colourShaderModelID, 1, GL_FALSE, &model[0][0]);
					groundPlane->Draw(vec3(i * 10, 0, j * 10), colourShaderColourID, goalColour);
				}
			}
		}

		// Swap buffers
		glfwSwapBuffers(window); // REDRAW
		glfwPollEvents(); // INPUT
	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	delete groundPlane;
	delete player;

	//system("PAUSE");
}