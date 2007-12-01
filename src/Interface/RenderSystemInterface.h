#	pragma once

struct	TextureDesc
{
	const char *	name;
	size_t			filter;

	void*			buffer;
	size_t			size;
};

class RenderImageInterface
{
public:
	virtual size_t getWidth() const = 0;
	virtual size_t getHeight() const = 0;
	virtual void writeToFile( const char* _filename ) = 0;
};

class	RenderSystemInterface
{
public:

	// Render frame into _image
	// int rect[4] - rectangle represents desired frame area in pixels
	virtual void render( RenderImageInterface* _image, const int* rect = 0 ) = 0;

	// ��������� ���������� �������� ����, ������� ������: ������2. 
	virtual void setContentResolution( const float * _resolution ) = 0;
	// ������� ������: ������� 4 �� 4
	virtual	void setProjectionMatrix( const float * _projection ) = 0;
	virtual	void setViewMatrix( const float * _view ) = 0;
	virtual	void setWorldMatrix( const float * _world ) = 0;
	// create empty render image
	virtual RenderImageInterface * createImage( const char* _name, unsigned int _width, unsigned int _height ) = 0;
	// �������� �����������
	virtual RenderImageInterface * loadImage( const TextureDesc& _desc ) = 0;
	// �������� �����������
	virtual void releaseImage( RenderImageInterface * _image ) = 0;
	// ��������� �����������
	virtual void renderImage(		
		const float * _transform, // ������� 3 �� 3
		const float * _offset,	  // ��������, ������2
		const float * _uv,		  // ���������� ����������, ������4, u0, v0, u1, v1
		const float * _size,	  // ������ �����������, ������2
		unsigned int _color,	  // ����, ������� RGBA
		const RenderImageInterface * _image) = 0;

	virtual void renderImage(		
		const float * _transform, 
		const float * _a,
		const float * _b,
		const float * _c,
		const float * _d,
		const float * _uv,
		unsigned int _color, 
		const RenderImageInterface * _image) = 0;

	virtual	void	beginLayer() = 0;
	virtual	void	endLayer() = 0;
};

bool initInterfaceSystem(RenderSystemInterface** _ptrRenderSystem);
void releaseInterfaceSystem(RenderSystemInterface* _ptrRenderSystem);