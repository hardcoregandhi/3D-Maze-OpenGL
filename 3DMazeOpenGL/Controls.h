#pragma once

// Include GLFW
#include <GLFW\glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

// Initial position : on +Z
glm::vec3 position = glm::vec3(10, 16.0, 8.27679062);

vec2 playerPosition;
vec2 prevPlayerPosition;


glm::mat4 getViewMatrix() {
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix() {
	return ProjectionMatrix;
}
glm::vec3 getPositionVec() {
	return position;
}

// Initial horizontal angle : toward -Z
float horizontalAngle = -0.3f;
// Initial vertical angle : none
float verticalAngle = 4.5f;
// Initial Field of View
float initialFoV = 70.0f;

float speed = 30.0f; // 3 units / second
float mouseSpeed = 0.0005f;

//one press checks
bool upPressed = false;
bool downPressed = false;
bool leftPressed = false;
bool rightPressed = false;

void computeMatricesFromInputs(GLFWwindow* window) {

	for (int i = 0; i < MAZE_BOUNDS; i++)
	{
		for (int j = 0; j < MAZE_BOUNDS; j++)
		{
			if (maze[i][j] == 5)
				playerPosition = vec2(i, j);
		}
	}

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(1024 / 2 - xpos);
	verticalAngle += mouseSpeed * float(768 / 2 - ypos);

	//reset to 0
	if (horizontalAngle > two_pi<float>() || horizontalAngle < -two_pi<float>())
	{
		horizontalAngle = 0;
	}
	if (verticalAngle > two_pi<float>() || verticalAngle < -two_pi<float>())
	{
		verticalAngle = 0;
	}

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
		);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
		);

	// Up vector
	glm::vec3 up = glm::cross(right, direction);

	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position -= right * deltaTime * speed;
	}

	// Down
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		position.y += deltaTime * speed;
	}
	// Up
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		position.y -= deltaTime * speed;
		if (position.y < 0)
			position.y = 0;
	}
	//CONTROLLED CUBE
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && upPressed != true) {
		upPressed = true;
		if (playerPosition.y < MAZE_BOUNDS)
			playerPosition.y += 1;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && downPressed != true) {
		downPressed = true;
		if (playerPosition.y > 0)
			playerPosition.y -= 1;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && leftPressed != true) {
		leftPressed = true;
		if (playerPosition.x < MAZE_BOUNDS-1)
			playerPosition.x += 1;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && rightPressed != true) {
		rightPressed = true;
		if (playerPosition.x < MAZE_BOUNDS)
			playerPosition.x -= 1;
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE) {
		upPressed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE) {
		downPressed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE) {
		leftPressed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE) {
		rightPressed = false;
	}

	if (maze[(int)playerPosition.x][(int)playerPosition.y] == 9)
	{
		std::cout << "Maze Completed! Resetting..." << std::endl;
		playerPosition = prevPlayerPosition;
		maze[(int)playerPosition.x][(int)playerPosition.y] = 0;
		maze[(int)playerStartingPosition.x][(int)playerStartingPosition.y] = 5;
	}
	else if (playerPosition != prevPlayerPosition && maze[(int)playerPosition.x][(int)playerPosition.y] != 1)
	{
		maze[(int)playerPosition.x][(int)playerPosition.y] = 5;
		maze[(int)prevPlayerPosition.x][(int)prevPlayerPosition.y] = 0;
	}
	else
		playerPosition = prevPlayerPosition;

		prevPlayerPosition = playerPosition;

	float FoV = initialFoV;//

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 10000.0f);
	// Camera matrix
	ViewMatrix = glm::lookAt(
		position,
		position + direction,
		up
		);

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}