/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** System layer API
*/

#	include "Config/Typedef.h"

#ifndef HGE_H
#define HGE_H


#include <windows.h>
#include <vector>
#define HGE_VERSION 0x180

#ifdef HGEDLL
#define EXPORT  __declspec(dllexport)
#else
#define EXPORT
#endif

#define CALL  __stdcall

#ifdef __BORLANDC__
 #define floorf (float)floor
 #define sqrtf (float)sqrt
 #define acosf (float)acos
 #define atan2f (float)atan2
 #define cosf (float)cos
 #define sinf (float)sin
 #define powf (float)pow
 #define fabsf (float)fabs

 #define min(x,y) ((x) < (y)) ? (x) : (y)
 #define max(x,y) ((x) > (y)) ? (x) : (y)
#endif


/*
** Common data types
*/
#ifndef DWORD
typedef unsigned long       DWORD;
typedef unsigned short      WORD;
typedef unsigned char       BYTE;
#endif


/*
** Common math constants
*/
#ifndef M_PI
#define M_PI	3.14159265358979323846f
#define M_PI_2	1.57079632679489661923f
#define M_PI_4	0.785398163397448309616f
#define M_1_PI	0.318309886183790671538f
#define M_2_PI	0.636619772367581343076f
#endif


/*
** HGE Handle types
*/
typedef void * HTEXTURE;
typedef void * HTARGET;

/*
** Hardware color macros
*/
#define ARGB(a,r,g,b)	((DWORD(a)<<24) + (DWORD(r)<<16) + (DWORD(g)<<8) + DWORD(b))
#define GETA(col)		((col)>>24)
#define GETR(col)		(((col)>>16) & 0xFF)
#define GETG(col)		(((col)>>8) & 0xFF)
#define GETB(col)		((col) & 0xFF)
#define SETA(col,a)		(((col) & 0x00FFFFFF) + (DWORD(a)<<24))
#define SETR(col,r)		(((col) & 0xFF00FFFF) + (DWORD(r)<<16))
#define SETG(col,g)		(((col) & 0xFFFF00FF) + (DWORD(g)<<8))
#define SETB(col,b)		(((col) & 0xFFFFFF00) + DWORD(b))


/*
** HGE Blending constants
*/
#define	BLEND_COLORADD		1
#define	BLEND_COLORMUL		0
#define	BLEND_ALPHABLEND	2
#define	BLEND_ALPHAADD		0
#define	BLEND_ZWRITE		4
#define	BLEND_NOZWRITE		0

#define BLEND_DEFAULT		(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE)
#define BLEND_DEFAULT_Z		(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_ZWRITE)

enum hgeBlendState
{
	BLEND_ZERO               = 1,
	BLEND_ONE                = 2,
	BLEND_SRCCOLOR           = 3,
	BLEND_INVSRCCOLOR        = 4,
	BLEND_SRCALPHA           = 5,
	BLEND_INVSRCALPHA        = 6,
	BLEND_DESTALPHA          = 7,
	BLEND_INVDESTALPHA       = 8,
	BLEND_DESTCOLOR          = 9,
	BLEND_INVDESTCOLOR       = 10,
	BLEND_SRCALPHASAT        = 11,
	BLEND_BOTHSRCALPHA       = 12,
	BLEND_BOTHINVSRCALPHA    = 13
};

/*
** HGE System state constants
*/
enum hgeBoolState
{
	HGE_WINDOWED		= 1,    // bool		run in window?		(default: false)
	HGE_ZBUFFER			= 2,    // bool		use z-buffer?		(default: false)
	HGE_TEXTUREFILTER	= 3,    // bool		texture filtering?	(default: true)
	
	HGE_USESOUND		= 4,    // bool		use BASS for sound?	(default: true)
	
	HGE_DONTSUSPEND		= 5,	// bool		focus lost:suspend?	(default: false)
	HGE_HIDEMOUSE		= 6,	// bool		hide system cursor?	(default: true)

	HGE_SHOWSPLASH		= 7,	// bool		hide system cursor?	(default: true)

	HGEBOOLSTATE_FORCE_DWORD = 0x7FFFFFFF
};

enum hgeHwndState
{
	HGE_HWND			= 15,	// int		window handle: read only
	HGE_HWNDPARENT		= 16,	// int		parent win handle	(default: 0)
	
	HGEHWNDSTATE_FORCE_DWORD = 0x7FFFFFFF
};

enum hgeIntState
{
	HGE_SCREENWIDTH		= 17,   // int		screen width		(default: 800)
	HGE_SCREENHEIGHT	= 18,   // int		screen height		(default: 600)
	HGE_SCREENBPP		= 19,   // int		screen bitdepth		(default: 32) (desktop bpp in windowed mode)
	
	HGE_SAMPLERATE		= 20,   // int		sample rate			(default: 44100)
	HGE_FXVOLUME		= 21,   // int		global fx volume	(default: 100)
	HGE_MUSVOLUME		= 22,   // int		global music volume	(default: 100)
	HGE_STREAMVOLUME	= 23,   // int		global music volume	(default: 100)
	
	HGE_FPS				= 24,	// int		fixed fps			(default: HGEFPS_UNLIMITED)

	HGE_POWERSTATUS		= 25,   // int		battery life percent + status
	
	HGEINTSTATE_FORCE_DWORD = 0x7FFFFFF
};

/*
** Callback protoype used by HGE
*/
typedef bool (*hgeCallback)();


/*
** HGE_FPS system state special constants
*/
#define HGEFPS_UNLIMITED	0
#define HGEFPS_VSYNC		-1


/*
** HGE_POWERSTATUS system state special constants
*/
#define HGEPWR_AC			-1
#define HGEPWR_UNSUPPORTED	-2


/*
** HGE Primitive type constants
*/
#define HGEPRIM_LINES		2
#define HGEPRIM_TRIPLES		3
#define HGEPRIM_QUADS		4
#define HGEPRIM_MESH		5


/*
** HGE Vertex structure
*/
struct hgeVertex
{
	float			x, y;		// screen position    
	float			z;			// Z-buffer depth 0..1
	DWORD			col;		// color
	float			tx, ty;		// texture coordinates
};

struct mengeVertex
{
	float x, y, z;
	float nx, ny, nz;
	DWORD col;
	float tx, ty;
};

/*
** HGE Triple structure
*/
struct hgeTriple
{
	hgeVertex		v[3];
	HTEXTURE		tex;
	int				blend;
};


/*
** HGE Quad structure
*/
struct hgeQuad
{
	hgeVertex		v[4];
	HTEXTURE		tex;
	//int				blend;
	hgeBlendState   srcBlend;
	hgeBlendState	dstBlend;
};

namespace Menge
{
	class LogSystemInterface;
}

class Gfx_Listener
{
public:
	virtual void onRestoreDevice() = 0;
};
/*
** HGE Interface class
*/
class HGE
{
public:
	virtual int			CALL    GetNumDips() = 0;
	virtual	void		CALL	Release() = 0;

	virtual bool		CALL	System_Initiate( Menge::LogSystemInterface* _logSystem ) = 0;
	virtual void		CALL	System_Shutdown() = 0;
	virtual Menge::TCharA*		CALL	System_GetErrorMessage() = 0;
	virtual	void		CALL	System_Log(const Menge::TCharA *format, ...) = 0;
	virtual void		CALL	System_Snapshot(const char *filename=0) = 0;

private:
	virtual void		CALL	System_SetStateBool  (hgeBoolState   state, bool        value) = 0;
	virtual void		CALL	System_SetStateHwnd  (hgeHwndState   state, HWND        value) = 0;
	virtual void		CALL	System_SetStateInt   (hgeIntState    state, int         value) = 0;
	virtual bool		CALL	System_GetStateBool  (hgeBoolState   state) = 0;
	virtual HWND		CALL	System_GetStateHwnd  (hgeHwndState   state) = 0;
	virtual int			CALL	System_GetStateInt   (hgeIntState    state) = 0;

public:
	inline void					System_SetState(hgeBoolState   state, bool        value) { System_SetStateBool  (state, value); }
	inline void					System_SetState(hgeHwndState   state, HWND        value) { System_SetStateHwnd  (state, value); }
	inline void					System_SetState(hgeIntState    state, int         value) { System_SetStateInt   (state, value); }
	inline bool					System_GetState(hgeBoolState   state) { return System_GetStateBool  (state); }
	inline HWND					System_GetState(hgeHwndState   state) { return System_GetStateHwnd  (state); }
	inline int					System_GetState(hgeIntState    state) { return System_GetStateInt   (state); }
	
	virtual bool		CALL	Gfx_CreateRenderWindow() = 0;

	virtual bool		CALL	Gfx_BeginScene(HTARGET target=0) = 0;
	virtual void		CALL	Gfx_EndScene() = 0;
	virtual void		CALL	Gfx_SwapBuffers() = 0;
	virtual void		CALL	Gfx_Clear(DWORD color) = 0;
	virtual void		CALL	Gfx_RenderLine(float x1, float y1, float x2, float y2, DWORD color=0xFFFFFFFF, float z=0.5f) = 0;
	virtual void		CALL	Gfx_RenderTriple(const hgeTriple *triple) = 0;
	virtual void		CALL	Gfx_RenderQuad(const hgeQuad *quad) = 0;
	virtual hgeVertex*	CALL	Gfx_StartBatch(int prim_type, HTEXTURE tex, int blend, int *max_prim) = 0;
	virtual void		CALL	Gfx_FinishBatch(int nprim) = 0;
	virtual void		CALL	Gfx_RenderMesh( const mengeVertex* _vertices, size_t _verticesNum, const unsigned short* _indices, size_t _indicesNum, HTEXTURE _htex ) = 0;
	virtual void		CALL	Gfx_SetClipping(int x=0, int y=0, int w=0, int h=0) = 0;
	virtual void		CALL	Gfx_SetTransform(float x=0, float y=0, float dx=0, float dy=0, float rot=0, float hscale=0, float vscale=0) = 0; 

	virtual void		CALL	Gfx_Snapshot( HTEXTURE tex, RECT _rect ) = 0;
	virtual void		CALL	Gfx_ChangeMode( int _width, int _height, int _bpp, bool _fullscreen ) = 0;
	virtual const std::vector<int>& CALL	Gfx_GetModeList() = 0;
	virtual void		CALL	Gfx_SetProjectionMatrix( const float* _projMat ) = 0;
	virtual void		CALL	Gfx_SetViewMatrix( const float* _viewMat ) = 0;
	virtual void		CALL	Gfx_SetWorldMatrix( const float* _worldMat ) = 0;
	virtual void		CALL	Gfx_SetTextureMatrix( const float* _texMat ) = 0;
	virtual void		CALL	Gfx_Prepare2D() = 0;
	virtual void		CALL	Gfx_Prepare3D() = 0;
	virtual void		CALL	Gfx_SetBlendState( hgeBlendState _srcBlend, hgeBlendState _dstBlend ) = 0;
	virtual void		CALL	Gfx_SetListener( Gfx_Listener* _listener ) = 0;
	virtual bool		CALL	Gfx_SupportNPOT() const = 0;

	virtual HTARGET		CALL	Target_Create(int width, int height, bool zbuffer) = 0;
	virtual void		CALL	Target_Free(HTARGET target) = 0;
	virtual HTEXTURE	CALL	Target_GetTexture(HTARGET target) = 0;

	virtual HTEXTURE	CALL	Texture_Create(int width, int height, int _format) = 0;
	virtual HTEXTURE	CALL	Texture_Load(const char *filename, DWORD size=0, bool bMipmap=false) = 0;
	virtual void		CALL	Texture_LoadRawData( HTEXTURE _hTex, const char* data, int _pitch, int _width, int _height, int _format ) = 0;
	virtual void		CALL	Texture_Free(HTEXTURE tex) = 0;
	virtual int			CALL	Texture_GetWidth(HTEXTURE tex, bool bOriginal=false) = 0;
	virtual int			CALL	Texture_GetHeight(HTEXTURE tex, bool bOriginal=false) = 0;
	virtual DWORD*		CALL	Texture_Lock(HTEXTURE tex, int* _pitch, bool bReadOnly=true, int left=0, int top=0, int width=0, int height=0) = 0;
	virtual void		CALL	Texture_Unlock(HTEXTURE tex) = 0;
};

extern "C" { EXPORT HGE * CALL hgeCreate(int ver); }

#endif

