#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <gl\glew.h>
using namespace glm;
class GroundPlane
{
public:
	GroundPlane(GLuint _programID);
	~GroundPlane();

	void Draw(vec3 position, GLuint colourID, vec3 _colour);

private:
	GLuint programID;
	GLuint ColourID;
	GLuint vertexbuffer;
	GLuint VertexArrayID;
	GLuint modelMatrixID;

	GLuint colourShaderModelID;
	GLuint colourShaderViewID;
	GLuint colourShaderProjectionID;
	GLuint colourShaderColourID;

};

