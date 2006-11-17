#	pragma once

#	include "interfaces.h"
#	include <map>

#	include "RenderImage.h"

class	D3D9Font 
	: public RenderFontInterface
{
public:
	D3D9Font( LPDIRECT3DDEVICE9	 dev, const FontDesc&	_fontDesc  );
	~D3D9Font();

public:
	float			getWidth() const;
    float			getHeight() const;
    float			getInterval() const;
    unsigned int	getColor() const;

	mt::vec4f&	_getChar( char id );
	LPDIRECT3DTEXTURE9	_getTexPointer() const;

private:
	mt::vec4f		mLetters[256];
	LPDIRECT3DTEXTURE9	mTexPointer;
	unsigned int		mNumChars;
	unsigned int		mHChar;
	unsigned int		mWSpace;
	unsigned int		mWBroadestChar;
	unsigned int		mColor;
};