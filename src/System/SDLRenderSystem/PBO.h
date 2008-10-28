#	include "OGLRenderSystem.h"

class CPBO
{
public:
	CPBO();
	~CPBO();

public:
  	bool Init(int size);
	bool Done();
	
	unsigned char * Map(bool _discard);
	bool Unmap();
	
	unsigned int GetMaxSize();
	
	void Bind();
	void UnBind();

private:
	GLuint id;
	unsigned char* data;
	int maxsize;
};