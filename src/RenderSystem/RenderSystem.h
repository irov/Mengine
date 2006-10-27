#	pragma once

#	include "interfaces.h"

#	include "RenderImage.h"
#	include "RenderFont.h"

#	include "math/vec3.h"
#	include "math/mat3.h"
#	include "math/mat4.h"

#	include <map>
#	include <vector>


/*	
	Structure for holding batching of font.
	All fonts flushing in Direct3d9RenderSystem::endSceneDrawing().
*/
struct	FontBatch
{
	mt::vec2f	pos;
	std::string		text;
	D3D9Font*		font;
};

/*
	All resources are D3DPOOL_MANAGED.

	internal:
		_initRenderSystem() - init RS. Need for Lost Device.
		_initBatching() - init	VB, IB batching.
		_flushFonts() - flush all fonts batches.
*/
class	Direct3d9RenderSystem 
	: public RenderSystemInterface
{
public:
	Direct3d9RenderSystem();
	~Direct3d9RenderSystem();

public:
	bool	createDisplay(unsigned int _width, unsigned int _height, unsigned int _bits, bool _fullScreen);
	bool	beginSceneDrawing(unsigned long _color);
	bool	endSceneDrawing();

	void	drawLine(const mt::vec2f& p1, const mt::vec2f& p2, float width, unsigned long color) override;


    void	renderImage(const mt::mat3f& _transform, unsigned int _mixedColor,RenderImageInterface* _rmi);
	void	releaseRenderImage(RenderImageInterface* _rmi);
    void	renderText(mt::vec2f _pos, RenderFontInterface* _font, const std::string& _text);
	void	releaseRenderFont(RenderFontInterface* _fnt);

	RenderImageInterface* loadImage(const TextureDesc&	_desc);
	RenderFontInterface* loadFont(const FontDesc& _desc);

private:
	/*	
		size of	VB for batching.
	*/
	static const int BATCH_BUFFER_SIZE = 1000;

	LPDIRECT3D9					mDirect3D9;
	LPDIRECT3DDEVICE9			mDeviceD3D9;

	LPDIRECT3DVERTEXBUFFER9		mVBDynamic;

	UINT						mSizeOf4Verts;
	UINT						mSizeOfVert;

	std::vector<D3D9Vertex>		mFontVertices;

	LPDIRECT3DVERTEXBUFFER9		mBatchVB;
	LPDIRECT3DINDEXBUFFER9		mBatchIB;
	UINT						mNumBatchVertices;
	LPDIRECT3DTEXTURE9			mCurrentTex;

	LPD3DXLINE					m_pLine;

	size_t						mWidth;
	size_t						mHeight;
	size_t						mBits;
	bool						mFullScreen;
	bool						mDeviceLost;

	D3DPRESENT_PARAMETERS		mPresent;

	typedef std::multimap<LPDIRECT3DTEXTURE9,FontBatch> TMultimapFontBatch;

	TMultimapFontBatch			mBuffersToFlush;

	bool						_initRenderSystem();
	bool						_initBatching();
	void						_flushFonts();
};