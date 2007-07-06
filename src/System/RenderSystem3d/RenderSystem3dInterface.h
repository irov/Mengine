#	pragma once

enum DECLTYPE
{
	DECLFLOAT2,
	DECLFLOAT3,
	DECLFLOAT4
};

enum DECLUSAGE
{
	DECLPOS,
	DECLTEX,
	DECLNRM,
	DECLCOL
};

enum RenderOperation
{
	TRIANGLELIST,
	TRIANGLESTRIP
};

class VertexDeclaration
{
public:
	virtual ~VertexDeclaration(){};
	virtual void	insert(size_t stream, size_t offset, DECLTYPE type, DECLUSAGE usage, size_t index) = 0;
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

	RenderOperation	getRenderOp() const
	{
		return m_type;
	}

protected:
	VertexData * m_vertexData;
	IndexData * m_indexData;
	RenderOperation	m_type;
};

class	Texture
{
public:
	virtual ~Texture(){};
};

typedef void (*renderFunction)();

class	RenderSystem 
{
public:
	virtual ~RenderSystem(){};
	virtual bool	createDisplay(int _width, int _height, int _bits, bool _fullScreen) = 0;
	virtual void	setRenderCallback(renderFunction _rf) = 0;
	virtual void	drawPrimitive(const PrimitiveData& _pd) = 0;
	virtual void	update() = 0;
	virtual VertexData * createVertexData() = 0;
	virtual IndexData * createIndexData() = 0;
	virtual VertexDeclaration * createVertexDeclaration() = 0;
	virtual Texture * createTexture() = 0;
};