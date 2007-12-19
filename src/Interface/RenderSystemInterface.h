#	pragma once

struct	TextureDesc
{
	const char *	name;
	size_t			filter;

	void*			buffer;
	size_t			size;
};

enum EBlendFactor 
{
	BF_ONE,
	BF_ZERO,
	BF_DEST_COLOUR,
	BF_SOURCE_COLOUR,
	BF_ONE_MINUS_DEST_COLOUR,
	BF_ONE_MINUS_SOURCE_COLOUR,
	BF_DEST_ALPHA,
	BF_SOURCE_ALPHA,
	BF_ONE_MINUS_DEST_ALPHA,
	BF_ONE_MINUS_SOURCE_ALPHA
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
	// Renders One Frame
	virtual void render() = 0; 
	// установка разрешения контента игры, входные данные: вектор2. 
	virtual void setContentResolution( const float * _resolution ) = 0;
	// входные данные: матрица 4 на 4
	virtual	void setProjectionMatrix( const float * _projection ) = 0;
	virtual	void setViewMatrix( const float * _view ) = 0;
	virtual	void setWorldMatrix( const float * _world ) = 0;
	// create empty render image
	virtual RenderImageInterface * createImage( const char* _name, unsigned int _width, unsigned int _height ) = 0;
	// create render target image
	virtual RenderImageInterface * createRenderTargetImage( const char* _name, unsigned int _width, unsigned int _height ) = 0;
	// загрузка изображения
	virtual RenderImageInterface * loadImage( const TextureDesc& _desc ) = 0;
	// удаления изображения
	virtual void releaseImage( RenderImageInterface * _image ) = 0;
	// отрисовка изображения
	virtual void renderImage(		
		const float * _transform, // матрица 3 на 3
		const float * _offset,	  // смещение, вектор2
		const float * _uv,		  // текстурные координаты, вектор4, u0, v0, u1, v1
		const float * _size,	  // размер изображения, вектор2
		unsigned int _color,	  // цвет, порядок RGBA
		const RenderImageInterface * _image,
		EBlendFactor _src,
		EBlendFactor _dst) = 0;

	virtual void renderImage(		
		const float * _transform, 
		const float * _a,
		const float * _b,
		const float * _c,
		const float * _d,
		const float * _uv,
		unsigned int _color, 
		const RenderImageInterface * _image,
		EBlendFactor _src,
		EBlendFactor _dst) = 0;

	virtual	void	beginLayer() = 0;
	virtual	void	endLayer() = 0;

	virtual void	setFullscreenMode( bool _fullscreen ) = 0;
	virtual void	setViewportDimensions( float _width, float _height, float _renderFactor ) = 0;

};

bool initInterfaceSystem(RenderSystemInterface** _ptrRenderSystem);
void releaseInterfaceSystem(RenderSystemInterface* _ptrRenderSystem);