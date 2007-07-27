#	pragma once

#	include <string>

#	include "libs/math/mat4.h"
#	include "libs/math/mat3.h"
#	include "libs/math/vec4.h"
#	include "libs/math/vec2.h"
#	include "libs/math/vec3.h"

enum eDeclType
{
	DECLFLOAT2,
	DECLFLOAT3,
	DECLFLOAT4
};

enum eDeclUsage
{
	DECLPOS,
	DECLTEX,
	DECLNRM,
	DECLCOL
};

enum eRenderOp
{
	TRIANGLELIST,
	TRIANGLESTRIP
};

class VertexDeclaration
{
public:
	virtual ~VertexDeclaration(){};
	virtual void	insert(size_t stream, size_t offset, eDeclType type, eDeclUsage usage, size_t index) = 0;
};

class IndexData
{
public:
	virtual ~IndexData(){};
	virtual void	createIndexBuffer(size_t _count, size_t _start = 0) = 0;
	virtual size_t	getIndexStart() const = 0;
	virtual size_t	getIndexCount() const = 0;
	virtual void	lock(unsigned short* &_indecies, size_t offset, size_t _count) = 0;
	virtual void	unlock() = 0;
};

class VertexData
{
public:
	virtual ~VertexData(){};
	virtual	void	setVertexDeclaration(VertexDeclaration * _vd) = 0;
    virtual void	createVertexBuffer(size_t _count, size_t _vertexSizeInBytes, size_t _start = 0) = 0;
	virtual size_t	getVertexSize() const = 0;
	virtual size_t	getVertexStart() const = 0;
	virtual size_t	getVertexCount() const = 0;
	virtual void	lock(float* &_vertices, size_t offset, size_t _count, size_t _vertexSizeInBytes) = 0;
	virtual void	unlock() = 0;
};

class PrimitiveData 
{
public:
	PrimitiveData()
		: m_vertexData(0)
		, m_indexData(0)
		, m_type(TRIANGLELIST)
	{}

	~PrimitiveData()
	{
		delete m_vertexData;
		delete m_indexData;
	}

	void	setIndexData(IndexData * _indexData)
	{
		m_indexData = _indexData;
	}

	void	setVertexData(VertexData * _vertexData)
	{
		m_vertexData = _vertexData;
	}

	VertexData * getVertexData() const
	{
		return m_vertexData;
	}

	IndexData * getIndexData() const
	{
		return m_indexData;
	}

	eRenderOp	getRenderOp() const
	{
		return m_type;
	}

protected:
	VertexData * m_vertexData;
	IndexData * m_indexData;
	eRenderOp	m_type;
};

class RenderImageInterface
{
public:
	virtual int getWidth() const = 0;
	virtual int getHeight() const = 0;
};

struct	TextureDesc
{
	void*			buffer;
	size_t			size;
	int				filter;
};

class RenderFontInterface
{
public:
	virtual float			getHeight() const = 0;
	virtual unsigned int	getColor() const = 0;
	virtual float			getCharWidth(char id) const = 0;
};

struct FontCharDesc
{
	mt::vec4f		uv;
	float			width;
};

struct FontDesc
{
	TextureDesc		texDesc;
	FontCharDesc	chars[256];
	unsigned long	color;
	float			height;
};

class	RenderSystemInterface 
{
public:
	virtual ~RenderSystemInterface(){};

	virtual bool beginScene(int _color) = 0;
	virtual bool endScene() = 0;

	virtual void	setMaterialColor(unsigned char _ambient[4], 
		unsigned char _diffuse[4],
		unsigned char _specular[4]) = 0;

	virtual void	drawPrimitive(PrimitiveData * _pd) = 0;
	
	virtual VertexData * createVertexData() = 0;
	virtual IndexData * createIndexData() = 0;
	virtual VertexDeclaration * createVertexDeclaration() = 0;

	virtual void setTexture(RenderImageInterface * _tex) = 0;

	virtual RenderImageInterface* loadImage(const TextureDesc&	_desc) = 0;

	virtual void	renderImage(			
		const mt::mat3f& _transform, 
		const mt::vec2f& _offset,
		const mt::vec4f& _uv,
		const mt::vec2f& _size,
		unsigned int _mixedColor, 
		RenderImageInterface* _rmi) = 0;

	virtual void	releaseRenderImage(RenderImageInterface* _rmi) = 0;
	virtual RenderFontInterface* loadFont(const FontDesc&	_desc) = 0;
	virtual	void	renderText(const mt::vec2f& _pos, RenderFontInterface* _font, const std::string& _text) = 0;
	virtual	void	releaseRenderFont(RenderFontInterface* _fnt) = 0;

	virtual	void	setProjectionMatrix(const mt::mat4f& _projection) = 0;
	virtual	void	setViewMatrix(const mt::mat4f& _view) = 0;
	virtual	void	setWorldMatrix(const mt::mat4f& _world) = 0;

	virtual void	drawLine2D(const mt::vec2f& p1, const mt::vec2f& p2, unsigned long _color) = 0;
	virtual	void	drawLine3D(const mt::vec3f& p1, const mt::vec3f& p2, unsigned long _color) = 0;
	virtual void	drawBox(const mt::vec3f& _minEdge, const mt::vec3f & _maxEdge, unsigned long _color) = 0;
};

bool initInterfaceSystem(RenderSystemInterface** _ptrRenderSystem);
void releaseInterfaceSystem(RenderSystemInterface* _ptrRenderSystem);