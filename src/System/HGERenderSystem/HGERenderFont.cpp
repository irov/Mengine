#	include "HGERenderFont.h"

#	include <stdlib.h>
#	include <stdio.h>
#	include <math.h>
//////////////////////////////////////////////////////////////////////////
HGERenderFont::HGERenderFont(const std::string & _name, HGE * _hge)
	: m_hge(_hge)
	, m_height(0.0f)
	, m_texture(0)
{
	_fontGenerate(_name,20,true,false,false,32,126);
}
//////////////////////////////////////////////////////////////////////////
HGERenderFont::~HGERenderFont()
{
	if(m_texture)
	{
		m_hge->Texture_Free(m_texture);
	}
}
//////////////////////////////////////////////////////////////////////////
bool HGERenderFont::_placeSymbols(int _width, int _height, int leftRange,  int rigthRange)
{
	int x = 1;
	int y = 1;

	for(int j = leftRange; j<=rigthRange; j++ )
	{
		if(y + m_letters[j].h+1 >= _height)
		{
			return false;
		}

		if(x + m_letters[j].w + 1 >= _width)
		{
			x = 1;
			y += m_letters[j].h + 1;

			if(y + m_letters[j].h + 1 >= _height)
			{
				return false;
			}
		}

		m_letters[j].x = x;
		m_letters[j].y = y;

		x += m_letters[j].w + 1;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
HTEXTURE HGERenderFont::_fontGenerate(const std::string & _name, int _size, bool _bold, bool _italic, bool _antialias, int _leftRange, int _rigthRange )
{
	HBITMAP		hBM;
	BITMAPINFO	bmi;
	ABCFLOAT	abc;
	TEXTMETRIC	tTextMetrics;

	HFONT hFont = CreateFont(-_size, 0, 0, 0, (_bold) ? FW_BOLD : FW_NORMAL,
		_italic, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		(_antialias) ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, _name.c_str());

	if(!hFont)
	{
		return 0;
	}

	HDC hBMDC = CreateCompatibleDC(0);
	SetTextColor(hBMDC, RGB(255,255,255));
	SetBkColor(hBMDC, RGB(0,0,0));
	SetBkMode(hBMDC, TRANSPARENT);
	SetMapMode(hBMDC, MM_TEXT);
	SetTextAlign(hBMDC, TA_TOP);
	SelectObject(hBMDC, hFont);

	GetTextMetrics(hBMDC, &tTextMetrics);

	for (int j = 0; j < 256; j++ )
	{

		m_letters[j].x = 0;
		m_letters[j].y = 0;
		m_letters[j].a = 0;
		m_letters[j].c = 0;
		m_letters[j].w = 0;
		m_letters[j].h = 0;
	}

	for (int j = _leftRange; j <= _rigthRange; j++ )
	{
		GetCharABCWidthsFloat(hBMDC, j, j, &abc);
		m_letters[j].a = int(abc.abcfA)-1;
		m_letters[j].c = int(abc.abcfC)-1;
		m_letters[j].w = int(abc.abcfB)+2;
		m_letters[j].h = tTextMetrics.tmHeight;

		if(m_letters[j].h > m_height)
		{
			m_height=m_letters[j].h;
		}
	}

	int nWidth = 32;
	int nHeight = 32;

	for(;;)
	{
		if(_placeSymbols(nWidth, nHeight, _leftRange, _rigthRange)) 
		{
			break;
		}

		if(nWidth<=nHeight)
		{
			nWidth<<=1;
		}
		else 
		{
			nHeight<<=1;
		}

		if(nWidth > 1024 || nHeight > 1024)
		{
			DeleteObject(hFont);
			DeleteDC(hBMDC);
			return 0;
		}
	}

	memset(&bmi, 0, sizeof(BITMAPINFO));
	bmi.bmiHeader.biWidth = nWidth;
	bmi.bmiHeader.biHeight = -nHeight;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	DWORD	* pPixels = 0;

	hBM = CreateDIBSection(hBMDC, &bmi, DIB_RGB_COLORS, (void**)&pPixels, 0, 0);

	if(!hBM)
	{
		DeleteObject(hFont);
		DeleteDC(hBMDC);
		return 0;
	}

	SelectObject(hBMDC, hBM);

	for (int j = _leftRange; j <= _rigthRange; j++ )
	{
		char c = (char)j;
		TextOut(hBMDC, m_letters[j].x-m_letters[j].a, m_letters[j].y, &c, 1);
	}

	GdiFlush();

	m_texture = m_hge->Texture_Create(nWidth, nHeight);

	int pitch = 0;

	DWORD * pTexData = m_hge->Texture_Lock(m_texture, &pitch);

	for (int i = 0; i<nHeight; i++)
	{
		for (int j = 0; j<nWidth; j++)
		{
			DWORD dwPixel = pPixels[i*nWidth+j];
			dwPixel = 0xFFFFFF | ((dwPixel & 0xFF) << 24);
			pTexData[i*nWidth+j] = dwPixel;
		}
	}

	m_hge->Texture_Unlock(m_texture);

	DeleteObject(hBM);
	DeleteObject(hFont);
	DeleteDC(hBMDC);

	return m_texture;
}

void HGERenderFont::renderText(float _x, float _y, unsigned long _color, const std::string & _text)
{
	float	fx = _x;

	for(std::string::const_iterator it = _text.begin(); it != _text.end(); it++)
	{
		char letter = *it;

		if(letter == '\n')
		{
			_y += m_height;
			fx = _x;
		}
		else
		{
			int i = (unsigned char)letter;

			fx += m_letters[i].a;

			float tx1 = 0;
			float ty1 = 0;
			float tx2 = m_letters[i].w;
			float ty2 = m_letters[i].h;

			hgeQuad quad;

			quad.v[0].x = tx1 + fx; quad.v[0].y = ty1 + _y;
			quad.v[1].x = tx2 + fx; quad.v[1].y = ty1 + _y;
			quad.v[2].x = tx2 + fx; quad.v[2].y = ty2 + _y;
			quad.v[3].x = tx1 + fx; quad.v[3].y = ty2 + _y;

			quad.v[0].z = 1; quad.v[0].col = _color;
			quad.v[1].z = 1; quad.v[1].col = _color;
			quad.v[2].z = 1; quad.v[2].col = _color;
			quad.v[3].z = 1; quad.v[3].col = _color;

			for( int k = 0; k < 4; k++ )
			{
				quad.v[k].x = ::floorf( quad.v[k].x + 0.5f );
				quad.v[k].y = ::floorf( quad.v[k].y + 0.5f );
			}

			float width = m_hge->Texture_GetWidth(m_texture);
			float height = m_hge->Texture_GetHeight(m_texture);

			quad.v[0].tx = m_letters[i].x/width;
			quad.v[0].ty = m_letters[i].y/height;

			quad.v[1].tx = (m_letters[i].x+m_letters[i].w)/width;
			quad.v[1].ty = m_letters[i].y/height;

			quad.v[2].tx = (m_letters[i].x+m_letters[i].w)/width;
			quad.v[2].ty = (m_letters[i].y+m_letters[i].h)/height;

			quad.v[3].tx = m_letters[i].x/width; 
			quad.v[3].ty = (m_letters[i].y+m_letters[i].h)/height;
			quad.tex = m_texture;

			quad.srcBlend = BLEND_SRCALPHA;
			quad.dstBlend = BLEND_INVDESTALPHA;

			m_hge->Gfx_RenderQuad( &quad );

			fx += (m_letters[i].w+m_letters[i].c);
		}
	}
}