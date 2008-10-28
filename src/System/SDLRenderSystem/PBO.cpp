#	include "PBO.h"
//////////////////////////////////////////////////////////////////////////
CPBO::CPBO()
: id(0)
, data(NULL)
, maxsize(0)
{}
//////////////////////////////////////////////////////////////////////////
CPBO::~CPBO()
{

}
//////////////////////////////////////////////////////////////////////////
bool CPBO::Init( int size )
{
	glGenBuffers(1, &id);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, id);
	glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, size, NULL, GL_STATIC_DRAW);
	maxsize = size;
	return true;
}
//////////////////////////////////////////////////////////////////////////
void CPBO::Bind()
{
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, id);
}
//////////////////////////////////////////////////////////////////////////
void CPBO::UnBind()
{
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
}
//////////////////////////////////////////////////////////////////////////
bool CPBO::Done()
{
	if (data != NULL)
	{
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, id);
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB);
		data = NULL;
	}

	glDeleteBuffers(1, &id);
	return true;
}
//////////////////////////////////////////////////////////////////////////
unsigned char * CPBO::Map(bool _discard)
{
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, id);

	if(_discard)
	{
		glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, maxsize, NULL, GL_STREAM_DRAW);
	}

	data = static_cast<unsigned char*>(glMapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY));
	return data;
}
//////////////////////////////////////////////////////////////////////////
bool CPBO::Unmap()
{
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, id);
	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB);
	data = NULL;
	return true;
}
//////////////////////////////////////////////////////////////////////////
unsigned int CPBO::GetMaxSize()
{
	return maxsize;
}
//////////////////////////////////////////////////////////////////////////