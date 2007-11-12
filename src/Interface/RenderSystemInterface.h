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
};

class	RenderSystemInterface
{
public:
	virtual void render() = 0;

	virtual	void setProjectionMatrix( const float * _projection ) = 0;
	virtual	void setViewMatrix( const float * _view ) = 0;
	virtual	void setWorldMatrix( const float * _world ) = 0;

	virtual RenderImageInterface * loadImage( const TextureDesc& _desc ) = 0;
	virtual void releaseImage( RenderImageInterface * _image ) = 0;
	virtual void renderImage(		
		const float * _transform, 
		const float * _offset,
		const float * _uv,
		const float * _size,
		unsigned int _color, 
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