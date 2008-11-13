#	include "OGLUtils.h"
//////////////////////////////////////////////////////////////////////////
void	OGLUtils::identity(GLfloat _matrix[16])
{
	for (size_t i = 0; i < 16; i++)
	{
		_matrix[i] = 0;
	}

	_matrix[0] = _matrix[5] = _matrix[10] = _matrix[15] = 1;
}
//////////////////////////////////////////////////////////////////////////
void OGLUtils::makeGLMatrix(GLfloat gl_matrix[16], const float * m)
{
	size_t k = 0;
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			gl_matrix[k++] = m[j+i*4];
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void OGLUtils::multiplyMatrices(float * _out, float * _a, float * _b)
{
	_out[0] = _a[0] * _b[0] + _a[1] * _b[4]+ _a[2] * _b[8]+ _a[3] * _b[12];
	_out[1] = _a[0] * _b[1] + _a[1] * _b[5]+ _a[2] * _b[9]+ _a[3] * _b[13];
	_out[2] = _a[0] * _b[2] + _a[1] * _b[6]+ _a[2] * _b[10]+ _a[3] * _b[14];
	_out[3] = _a[0] * _b[3] + _a[1] * _b[7]+ _a[2] * _b[11]+ _a[3] * _b[15];
	_out[4] = _a[4] * _b[0] + _a[5] * _b[4]+ _a[6] * _b[8]+ _a[7] * _b[12];
	_out[5] = _a[4] * _b[1] + _a[5] * _b[5]+ _a[6] * _b[9]+ _a[7] * _b[13];
	_out[6] = _a[4] * _b[2] + _a[5] * _b[6]+ _a[6] * _b[10]+ _a[7] * _b[14];
	_out[7] = _a[4] * _b[3] + _a[5] * _b[7]+ _a[6] * _b[11]+ _a[7] * _b[15];
	_out[8] = _a[8] * _b[0] + _a[9] * _b[4]+ _a[10] * _b[8]+ _a[11] * _b[12];
	_out[9] = _a[8] * _b[1] + _a[9] * _b[5]+ _a[10] * _b[9]+ _a[11] * _b[13];
	_out[10] = _a[8] * _b[2] + _a[9] * _b[6]+ _a[10] * _b[10]+ _a[11] * _b[14];
	_out[11] = _a[8] * _b[3] + _a[9] * _b[7]+ _a[10] * _b[11]+ _a[11] * _b[15];
	_out[12] = _a[12] * _b[0] + _a[13] * _b[4]+ _a[14] * _b[8]+ _a[15] * _b[12];
	_out[13] = _a[12] * _b[1] + _a[13] * _b[5]+ _a[14] * _b[9]+ _a[15] * _b[13];
	_out[14] = _a[12] * _b[2] + _a[13] * _b[6]+ _a[14] * _b[10]+ _a[15] * _b[14];
	_out[15] = _a[12] * _b[3] + _a[13] * _b[7]+ _a[14] * _b[11]+ _a[15] * _b[15];
}
//////////////////////////////////////////////////////////////////////////
GLint OGLUtils::s_blendMengeToOGL(Menge::EBlendFactor _blend )
{
	switch(_blend)
	{
	case Menge::BF_ONE:
		return GL_ONE;
	case Menge::BF_ZERO:
		return GL_ZERO;
	case Menge::BF_DEST_COLOUR:
		return GL_DST_COLOR;
	case Menge::BF_SOURCE_COLOUR:
		return GL_SRC_COLOR;
	case Menge::BF_ONE_MINUS_DEST_COLOUR:
		return GL_ONE_MINUS_DST_COLOR;
	case Menge::BF_ONE_MINUS_SOURCE_COLOUR:
		return GL_ONE_MINUS_SRC_COLOR;
	case Menge::BF_DEST_ALPHA:
		return GL_DST_ALPHA;
	case Menge::BF_SOURCE_ALPHA:
		return GL_SRC_ALPHA;
	case Menge::BF_ONE_MINUS_DEST_ALPHA:
		return GL_ONE_MINUS_DST_ALPHA;
	case Menge::BF_ONE_MINUS_SOURCE_ALPHA:
		return GL_ONE_MINUS_SRC_ALPHA;
	};
	
	return GL_ONE;
}