#include "RenderFont.h"

D3D9Font::D3D9Font(LPDIRECT3DDEVICE9	_devD3D9, const FontDesc&	_fontDesc)
{
	std::copy(
		_fontDesc.chars,
		_fontDesc.chars+256,
		stdext::checked_array_iterator<mt::vec4f*>(mLetters,256));

	mColor = _fontDesc.mColor;

    if (FAILED(D3DXCreateTextureFromFileInMemoryEx(_devD3D9, _fontDesc.texDesc.buffer, _fontDesc.texDesc.size,
								D3DX_DEFAULT, D3DX_DEFAULT,
								1, 0,					
								_fontDesc.texDesc.haveAlpha ? D3DFMT_A8R8G8B8 : D3DFMT_R8G8B8,	
								D3DPOOL_MANAGED,	
								D3DX_FILTER_NONE,	
								D3DX_DEFAULT,		
								0,					
								NULL, NULL,
								&mTexPointer ) ) )

	{
		assert(!"D3DXCreateTextureFromFileInMemoryEx");
	}

	D3DSURFACE_DESC		descTexture;

	if(FAILED(mTexPointer->GetLevelDesc(0, &descTexture)))
	{
		assert(!"can't get desc!");
	}

	/*unsigned int* typed_buffer = reinterpret_cast< unsigned int* >( _fontDesc.fontDesc );

	*typed_buffer++;
	*typed_buffer++;
	*typed_buffer++;
	*typed_buffer++;
	*typed_buffer++;

	mNumChars = *typed_buffer++;
	mHChar = *typed_buffer++;
	mWSpace = *typed_buffer++;
	mWBroadestChar = *typed_buffer++;

	unsigned int ascii_number;
	unsigned int widthch;
	unsigned int x;
	unsigned int y;

	for (unsigned int i = 0; i < mNumChars; i++)
	{
		ascii_number = *typed_buffer++;
		x = *typed_buffer++;
		y = *typed_buffer++;
		widthch = *typed_buffer++;
		mLetters[ascii_number-33].x = (float)x / (float)descTexture.Width;
		mLetters[ascii_number-33].y = (float)y / (float)descTexture.Height;
		mLetters[ascii_number-33].z = ((float)(x + widthch)) / (float)descTexture.Width;
		mLetters[ascii_number-33].w = ((float)(y + mHChar)) / (float)descTexture.Height;
	}
	*/
}

LPDIRECT3DTEXTURE9	D3D9Font::_getTexPointer() const
{
	return	mTexPointer;
}

D3D9Font::~D3D9Font()
{
}

float D3D9Font::getWidth() const
{
	return mWBroadestChar;
}

float D3D9Font::getHeight() const
{
	return mHChar;
}

float D3D9Font::getInterval() const
{
	return mWSpace;
}

unsigned int D3D9Font::getColor() const
{
	return mColor;
}

mt::vec4f&	D3D9Font::_getChar( char id )
{
	return mLetters[id];
}