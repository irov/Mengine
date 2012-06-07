#	pragma once

#	include "OGLRenderSystem.h"

class OGLUtils
{
public:
	static GLint s_blendMengeToOGL(Menge::EBlendFactor _blend );
	static void makeGLMatrix(GLfloat gl_matrix[16], const float * m);
	static void	identity(GLfloat _matrix[16]);
	static void multiplyMatrices(float * _out, float * _a, float * _b);
};