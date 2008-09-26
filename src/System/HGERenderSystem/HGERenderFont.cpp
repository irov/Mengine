#	include "HGERenderFont.h"

#	include <stdlib.h>
#	include <stdio.h>
#	include <math.h>
#	include "Menge/Utils.h"
//////////////////////////////////////////////////////////////////////////
HGERenderFont::HGERenderFont(HGE * _hge)
	: m_hge(_hge)
	, m_height(0.0f)
	, m_texture(0)
{}
//////////////////////////////////////////////////////////////////////////
HGERenderFont::~HGERenderFont()
{
	if(m_texture)
	{
		m_hge->Texture_Free(m_texture);
	}
}
//////////////////////////////////////////////////////////////////////////
void HGERenderFont::fontGenerate( const Menge::String& _name, int _size, bool _bold, bool _italic, bool _antialias)
{
	m_texture = _fontGenerate(_name,_size,_bold,_italic,_antialias,0,255);
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
HTEXTURE HGERenderFont::_fontGenerate( const Menge::String& _name, int _size, bool _bold, bool _italic, bool _antialias, int _leftRange, int _rigthRange )
{
	//char sPath[MAX_PATH];
	//OLECHAR sUniPath[MAX_PATH + 1];

	//MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, _name.c_str(), -1, sUniPath,
	//	MAX_PATH);

	HFONT hFont = CreateFont(-_size, 0, 0, 0, (_bold) ? FW_BOLD : FW_NORMAL,
		_italic, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		(_antialias) ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, Menge::Utils::AToW( _name ).c_str());

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

	TEXTMETRIC	tTextMetrics;
	GetTextMetrics(hBMDC, &tTextMetrics);

	ABCFLOAT	abc;

	for (int j = _leftRange; j <= _rigthRange; j++ )
	{
		GetCharABCWidthsFloat(hBMDC, j, j, &abc);

		m_letters[j].a = abc.abcfA - 1;
		m_letters[j].c = abc.abcfC - 1;

		m_letters[j].w = abc.abcfB + 2;
		m_letters[j].h = tTextMetrics.tmHeight;

		if(m_letters[j].h > m_height)
		{
			m_height = m_letters[j].h;
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

	BITMAPINFO	bmi;
	memset(&bmi, 0, sizeof(BITMAPINFO));
	bmi.bmiHeader.biWidth = nWidth;
	bmi.bmiHeader.biHeight = -nHeight;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	DWORD	* pPixels = 0;

	HBITMAP hBM = CreateDIBSection(hBMDC, &bmi, DIB_RGB_COLORS, (void**)&pPixels, 0, 0);

	if(!hBM)
	{
		DeleteObject(hFont);
		DeleteDC(hBMDC);
		return 0;
	}

	SelectObject(hBMDC, hBM);

	const size_t newsize = 100;
	size_t origsize = 1;
	size_t convertedChars = 0;
	wchar_t wcstring[newsize];

	for (int j = _leftRange; j <= _rigthRange; j++ )
	{
		char c = (char)j;
		mbstowcs( wcstring, &c, newsize);
	//	wcscat_s(wcstring, L" (wchar_t *)");
		TextOutW(hBMDC, m_letters[j].x-m_letters[j].a, m_letters[j].y, wcstring, 1);
	}

	GdiFlush();

	HTEXTURE texture = m_hge->Texture_Create(nWidth, nHeight);

	int pitch = 0;

	DWORD * pTexData = m_hge->Texture_Lock(texture, &pitch);

	for (int i = 0; i<nHeight; i++)
	{
		for (int j = 0; j<nWidth; j++)
		{
			DWORD dwPixel = pPixels[i*nWidth+j];
			dwPixel = 0xFFFFFF | ((dwPixel & 0xFF) << 24);
			pTexData[i*nWidth+j] = dwPixel;
		}
	}

	m_hge->Texture_Unlock(texture);

	float width = m_hge->Texture_GetWidth(texture);
	float height = m_hge->Texture_GetHeight(texture);

	for (int j = _leftRange; j <= _rigthRange; j++ )
	{
		m_letters[j].u0 = m_letters[j].x / width;
		m_letters[j].v0 = m_letters[j].y / height;
		m_letters[j].u1 = (m_letters[j].x + m_letters[j].w) / width;
		m_letters[j].v1 = (m_letters[j].y + m_letters[j].h) / height;
	}

	DeleteObject(hBM);
	DeleteObject(hFont);
	DeleteDC(hBMDC);

	return texture;
}

void HGERenderFont::renderText(float _x, float _y, unsigned long _color, const Menge::String& _text)
{
	float	widthOffset = _x;

	for( Menge::String::const_iterator it = _text.begin(); it != _text.end(); it++)
	{
		Menge::TChar letter = *it;

		if( letter == '\n' )
		{
			_y += m_height;
			widthOffset = _x;
			continue;
		};

		//FIX ME
		int i = (unsigned char)letter;

		widthOffset += m_letters[i].a;

		hgeQuad quad;

		quad.v[0].x =  widthOffset; 
		quad.v[0].y = _y;
		quad.v[0].z = 0;

		quad.v[1].x = m_letters[i].w + widthOffset;
		quad.v[1].y =  _y;
		quad.v[1].z = 0;

		quad.v[2].x = m_letters[i].w + widthOffset;
		quad.v[2].y = m_letters[i].h + _y;
		quad.v[2].z = 0;

		quad.v[3].x = widthOffset; 
		quad.v[3].y = m_letters[i].h + _y;
		quad.v[3].z = 0;

		quad.v[0].col = _color;
		quad.v[1].col = _color;
		quad.v[2].col = _color;
		quad.v[3].col = _color;

		for( int k = 0; k < 4; k++ )
		{
			quad.v[k].x = ::floorf( quad.v[k].x + 0.5f );
			quad.v[k].y = ::floorf( quad.v[k].y + 0.5f );
		}

		quad.v[0].tx = m_letters[i].u0;
		quad.v[0].ty = m_letters[i].v0;

		quad.v[1].tx = m_letters[i].u1;
		quad.v[1].ty = m_letters[i].v0;

		quad.v[2].tx = m_letters[i].u1;
		quad.v[2].ty = m_letters[i].v1;

		quad.v[3].tx = m_letters[i].u0; 
		quad.v[3].ty = m_letters[i].v1;

		quad.tex = m_texture;
		quad.srcBlend = BLEND_SRCALPHA;
		quad.dstBlend = BLEND_INVDESTALPHA;

		m_hge->Gfx_RenderQuad( &quad );

		widthOffset += (m_letters[i].w+m_letters[i].c);
	}
}