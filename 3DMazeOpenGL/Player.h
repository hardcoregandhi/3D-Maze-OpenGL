#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <gl\glew.h>
using namespace glm;
class Player
{
public:
	Player(GLuint _programID);
	~Player();

	void Draw();

private:
	GLuint programID;
	GLuint vertexbuffer;
	GLuint VertexArrayID;
};

