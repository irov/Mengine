#	pragma once

class RenderImageInterface
{
public:
	virtual size_t getWidth() const = 0;
	virtual size_t getHeight() const = 0;
};

struct	TextureDesc
{
	void*			buffer;
	size_t			size;

	const char * name;
	int				filter;
};

class	RenderSystemInterface
{
public:
	virtual void render() = 0;

	virtual	void setProjectionMatrix(const /*mat4f*/ float * _projection) = 0;
	virtual	void setViewMatrix(const /*mat4f*/ float * _view) = 0;
	virtual	void setWorldMatrix(const /*mat4f*/ float * _world) = 0;

	virtual RenderImageInterface* loadImage( const TextureDesc&	_desc ) = 0;
	virtual void releaseImage( RenderImageInterface* _rii ) = 0;
	virtual void renderImage(		
		const /*mat3*/ float * _transform, 
		const /*vec2f*/ float * _offset,
		const /*vec4f*/ float * _uv,
		const /*vec2f*/ float * _size,
		unsigned int _mixedColor, 
		RenderImageInterface* _rii) = 0;


	//virtual bool beginScene(int _color) = 0;
	//virtual bool endScene() = 0;

	//virtual void	setMaterialColor(unsigned char _ambient[4], 
	//	unsigned char _diffuse[4],
	//	unsigned char _specular[4]) = 0;

	//virtual void	drawPrimitive(PrimitiveData * _pd) = 0;
	//
	//virtual VertexData * createVertexData() = 0;
	//virtual IndexData * createIndexData() = 0;
	//virtual VertexDeclaration * createVertexDeclaration() = 0;

	//virtual void setTexture(RenderImageInterface * _tex) = 0;

	//virtual void	releaseRenderImage(RenderImageInterface* _rmi) = 0;
	//virtual RenderFontInterface* loadFont(const FontDesc&	_desc) = 0;
	//virtual	void	renderText(const mt::vec2f& _pos, RenderFontInterface* _font, const std::string& _text) = 0;
	//virtual	void	releaseRenderFont(RenderFontInterface* _fnt) = 0;



	//virtual void	drawLine2D(const mt::vec2f& p1, const mt::vec2f& p2, unsigned long _color) = 0;
	//virtual	void	drawLine3D(const mt::vec3f& p1, const mt::vec3f& p2, unsigned long _color) = 0;
	//virtual void	drawBox(const mt::vec3f& _minEdge, const mt::vec3f & _maxEdge, unsigned long _color) = 0;
};

bool initInterfaceSystem(RenderSystemInterface** _ptrRenderSystem);
void releaseInterfaceSystem(RenderSystemInterface* _ptrRenderSystem);