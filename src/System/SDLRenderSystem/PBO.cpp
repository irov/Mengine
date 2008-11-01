#	include "PBO.h"
//////////////////////////////////////////////////////////////////////////
PixelBufferObject::PixelBufferObject()
: m_id(0)
, m_data(NULL)
, m_maxsize(0)
{}
//////////////////////////////////////////////////////////////////////////
PixelBufferObject::~PixelBufferObject()
{

}
//////////////////////////////////////////////////////////////////////////
bool PixelBufferObject::init(int _size)
{
	glGenBuffers(1, &m_id);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, m_id);
	glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, _size, NULL, GL_STATIC_DRAW);
	m_maxsize = _size;
	return true;
}
//////////////////////////////////////////////////////////////////////////
void PixelBufferObject::bind()
{
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, m_id);
}
//////////////////////////////////////////////////////////////////////////
void PixelBufferObject::unbind()
{
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
}
//////////////////////////////////////////////////////////////////////////
bool PixelBufferObject::done()
{
	if (m_data != NULL)
	{
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, m_id);
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB);
		m_data = NULL;
	}

	glDeleteBuffers(1, &m_id);
	return true;
}
//////////////////////////////////////////////////////////////////////////
unsigned char * PixelBufferObject::map(bool _discard)
{
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, m_id);

	if(!_discard)
	{
		glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, m_maxsize, NULL, GL_STREAM_DRAW);
	}

	m_data = static_cast<unsigned char*>(glMapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY));

	return m_data;
}
//////////////////////////////////////////////////////////////////////////
bool PixelBufferObject::unmap()
{
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, m_id);
	// unmap the buffer from the CPU space so it can DMA
	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB);
	m_data = NULL;

	return true;
}
//////////////////////////////////////////////////////////////////////////
unsigned int PixelBufferObject::getMaxSize()
{
	return m_maxsize;
}
//////////////////////////////////////////////////////////////////////////