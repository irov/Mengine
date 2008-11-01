#	include "OGLRenderSystem.h"

class PixelBufferObject
{
public:
	PixelBufferObject();
	~PixelBufferObject();

public:
  	bool init(int _size);
	bool done();
	
	unsigned char * map(bool _discard);
	bool unmap();
	
	unsigned int getMaxSize();
	
	void bind();
	void unbind();

private:
	GLuint m_id;
	unsigned char * m_data;
	int m_maxsize;
};