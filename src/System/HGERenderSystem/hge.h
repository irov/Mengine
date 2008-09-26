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
//typedef DWORD HEFFECT;
//typedef DWORD HMUSIC;
//typedef DWORD HSTREAM;
//typedef DWORD HCHANNEL;


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

enum hgeFuncState
{
	HGE_FRAMEFUNC		= 8,    // bool*()	frame function		(default: NULL) (you MUST set this)
	HGE_RENDERFUNC		= 9,    // bool*()	render function		(default: NULL)
	HGE_FOCUSLOSTFUNC	= 10,   // bool*()	focus lost function	(default: NULL)
	HGE_FOCUSGAINFUNC	= 11,   // bool*()	focus gain function	(default: NULL)
	HGE_GFXRESTOREFUNC	= 12,   // bool*()	exit function		(default: NULL)
	HGE_EXITFUNC		= 13,   // bool*()	exit function		(default: NULL)
	
	HGEFUNCSTATE_FORCE_DWORD = 0x7FFFFFFF
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

enum hgeStringState
{
	HGE_ICON			= 26,   // char*	icon resource		(default: NULL)
	HGE_TITLE			= 27,   // char*	window title		(default: "HGE")
	
	HGE_INIFILE			= 28,   // char*	ini file			(default: NULL) (meaning no file)
	HGE_LOGFILE			= 29,   // char*	log file			(default: NULL) (meaning no file)

	HGESTRINGSTATE_FORCE_DWORD = 0x7FFFFFFF
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


/*
** HGE Input Event structure
*/
/*struct hgeInputEvent
{
	int		type;			// event type
	int		key;			// key code
	int		flags;			// event flags
	int		chr;			// character code
	int		wheel;			// wheel shift
	float	x;				// mouse cursor x-coordinate
	float	y;				// mouse cursor y-coordinate
};*/


/*
** HGE Input Event type constants
*/
/*#define INPUT_KEYDOWN		1
#define INPUT_KEYUP			2
#define INPUT_MBUTTONDOWN	3
#define INPUT_MBUTTONUP		4
#define INPUT_MOUSEMOVE		5
#define INPUT_MOUSEWHEEL	6
*/

/*
** HGE Input Event flags
*/
/*#define HGEINP_SHIFT		1
#define HGEINP_CTRL			2
#define HGEINP_ALT			4
#define HGEINP_CAPSLOCK		8
#define HGEINP_SCROLLLOCK	16
#define HGEINP_NUMLOCK		32
#define HGEINP_REPEAT		64
*/
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
	//virtual bool		CALL	System_Start() = 0;
	virtual Menge::TCharA*		CALL	System_GetErrorMessage() = 0;
	virtual	void		CALL	System_Log(const Menge::TCharA *format, ...) = 0;
	virtual bool		CALL	System_Launch(const char *url) = 0;
	virtual void		CALL	System_Snapshot(const char *filename=0) = 0;

private:
	virtual void		CALL	System_SetStateBool  (hgeBoolState   state, bool        value) = 0;
	virtual void		CALL	System_SetStateFunc  (hgeFuncState   state, hgeCallback value) = 0;
	virtual void		CALL	System_SetStateHwnd  (hgeHwndState   state, HWND        value) = 0;
	virtual void		CALL	System_SetStateInt   (hgeIntState    state, int         value) = 0;
	virtual void		CALL	System_SetStateString(hgeStringState state, const char *value) = 0;
	virtual bool		CALL	System_GetStateBool  (hgeBoolState   state) = 0;
	virtual hgeCallback	CALL	System_GetStateFunc  (hgeFuncState   state) = 0;
	virtual HWND		CALL	System_GetStateHwnd  (hgeHwndState   state) = 0;
	virtual int			CALL	System_GetStateInt   (hgeIntState    state) = 0;
	virtual const char*	CALL	System_GetStateString(hgeStringState state) = 0;

public:
	inline void					System_SetState(hgeBoolState   state, bool        value) { System_SetStateBool  (state, value); }
	inline void					System_SetState(hgeFuncState   state, hgeCallback value) { System_SetStateFunc  (state, value); }
	inline void					System_SetState(hgeHwndState   state, HWND        value) { System_SetStateHwnd  (state, value); }
	inline void					System_SetState(hgeIntState    state, int         value) { System_SetStateInt   (state, value); }
	inline void					System_SetState(hgeStringState state, const char *value) { System_SetStateString(state, value); }
	inline bool					System_GetState(hgeBoolState   state) { return System_GetStateBool  (state); }
	inline hgeCallback			System_GetState(hgeFuncState   state) { return System_GetStateFunc  (state); }
	inline HWND					System_GetState(hgeHwndState   state) { return System_GetStateHwnd  (state); }
	inline int					System_GetState(hgeIntState    state) { return System_GetStateInt   (state); }
	inline const char*			System_GetState(hgeStringState state) { return System_GetStateString(state); }
	
	//virtual void*		CALL	Resource_Load(const char *filename, DWORD *size=0) = 0;
	//virtual void		CALL	Resource_Free(void *res) = 0;
	//virtual bool		CALL	Resource_AttachPack(const char *filename, const char *password=0) = 0;
	//virtual void		CALL	Resource_RemovePack(const char *filename) = 0;
	//virtual void		CALL	Resource_RemoveAllPacks() = 0;
	//virtual char*		CALL	Resource_MakePath(const char *filename=0) = 0;
	//virtual char*		CALL	Resource_EnumFiles(const char *wildcard=0) = 0;
	//virtual char*		CALL	Resource_EnumFolders(const char *wildcard=0) = 0;

	//virtual	void		CALL	Ini_SetInt(const char *section, const char *name, int value) = 0;
	//virtual	int			CALL	Ini_GetInt(const char *section, const char *name, int def_val) = 0;
	//virtual	void		CALL	Ini_SetFloat(const char *section, const char *name, float value) = 0;
	//virtual	float		CALL	Ini_GetFloat(const char *section, const char *name, float def_val) = 0;
	//virtual	void		CALL	Ini_SetString(const char *section, const char *name, const char *value) = 0;
	//virtual	char*		CALL	Ini_GetString(const char *section, const char *name, const char *def_val) = 0;

	//virtual void		CALL	Random_Seed(int seed=0) = 0;
	//virtual int			CALL	Random_Int(int min, int max) = 0;
	//virtual float		CALL	Random_Float(float min, float max) = 0;

	//virtual float		CALL	Timer_GetTime() = 0;
	//virtual float		CALL	Timer_GetDelta() = 0;
	//virtual int			CALL	Timer_GetFPS() = 0;

	//virtual HEFFECT		CALL	Effect_Load(const char *filename, DWORD size=0) = 0;
	//virtual void		CALL	Effect_Free(HEFFECT eff) = 0;
	//virtual HCHANNEL	CALL 	Effect_Play(HEFFECT eff) = 0;
	//virtual HCHANNEL	CALL	Effect_PlayEx(HEFFECT eff, int volume=100, int pan=0, float pitch=1.0f, bool loop=false) = 0;

	//virtual HMUSIC		CALL	Music_Load(const char *filename, DWORD size=0) = 0;
	//virtual void		CALL	Music_Free(HMUSIC mus) = 0;
	//virtual HCHANNEL	CALL	Music_Play(HMUSIC mus, bool loop, int volume = 100, int order = -1, int row = -1) = 0;
	//virtual void		CALL	Music_SetAmplification(HMUSIC music, int ampl) = 0;
	//virtual int			CALL	Music_GetAmplification(HMUSIC music) = 0;
	//virtual int			CALL	Music_GetLength(HMUSIC music) = 0;
	//virtual void		CALL	Music_SetPos(HMUSIC music, int order, int row) = 0;
	//virtual bool		CALL	Music_GetPos(HMUSIC music, int *order, int *row) = 0;
	//virtual void		CALL	Music_SetInstrVolume(HMUSIC music, int instr, int volume) = 0;
	//virtual int			CALL	Music_GetInstrVolume(HMUSIC music, int instr) = 0;
	//virtual void		CALL	Music_SetChannelVolume(HMUSIC music, int channel, int volume) = 0;
	//virtual int			CALL	Music_GetChannelVolume(HMUSIC music, int channel) = 0;

	//virtual HSTREAM		CALL	Stream_Load(const char *filename, DWORD size=0) = 0;
	//virtual void		CALL	Stream_Free(HSTREAM stream) = 0;
	//virtual HCHANNEL	CALL	Stream_Play(HSTREAM stream, bool loop, int volume = 100) = 0;

	//virtual void		CALL	Channel_SetPanning(HCHANNEL chn, int pan) = 0;
	//virtual void		CALL 	Channel_SetVolume(HCHANNEL chn, int volume) = 0;
	//virtual void		CALL 	Channel_SetPitch(HCHANNEL chn, float pitch) = 0;
	//virtual void		CALL 	Channel_Pause(HCHANNEL chn) = 0;
	//virtual void		CALL 	Channel_Resume(HCHANNEL chn) = 0;
	//virtual void		CALL 	Channel_Stop(HCHANNEL chn) = 0;
	//virtual void		CALL 	Channel_PauseAll() = 0;
	//virtual void		CALL 	Channel_ResumeAll() = 0;
	//virtual void		CALL 	Channel_StopAll() = 0;
	//virtual bool		CALL	Channel_IsPlaying(HCHANNEL chn) = 0;
	//virtual float		CALL	Channel_GetLength(HCHANNEL chn) = 0;
	//virtual float		CALL	Channel_GetPos(HCHANNEL chn) = 0;
	//virtual void		CALL	Channel_SetPos(HCHANNEL chn, float fSeconds) = 0;
	//virtual void		CALL	Channel_SlideTo(HCHANNEL channel, float time, int volume, int pan = -101, float pitch = -1) = 0;
	//virtual bool		CALL	Channel_IsSliding(HCHANNEL channel) = 0;

	//virtual void		CALL	Input_GetMousePos(float *x, float *y) = 0;
	//virtual void		CALL	Input_SetMousePos(float x, float y) = 0;
	//virtual int			CALL	Input_GetMouseWheel() = 0;
	//virtual bool		CALL	Input_IsMouseOver() = 0;
	//virtual bool		CALL	Input_KeyDown(int key) = 0;
	//virtual bool		CALL	Input_KeyUp(int key) = 0;
	//virtual bool		CALL	Input_GetKeyState(int key) = 0;
	//virtual char*		CALL	Input_GetKeyName(int key) = 0;
	//virtual int			CALL	Input_GetKey() = 0;
	//virtual int			CALL	Input_GetChar() = 0;
	//virtual bool		CALL	Input_GetEvent(hgeInputEvent *event) = 0;
	
	virtual bool		CALL	Gfx_CreateRenderWindow() = 0;

	virtual bool		CALL	Gfx_BeginScene(HTARGET target=0) = 0;
	virtual void		CALL	Gfx_EndScene( bool _swapBuffers = true ) = 0;
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

	virtual HTARGET		CALL	Target_Create(int width, int height, bool zbuffer) = 0;
	virtual void		CALL	Target_Free(HTARGET target) = 0;
	virtual HTEXTURE	CALL	Target_GetTexture(HTARGET target) = 0;

	virtual HTEXTURE	CALL	Texture_Create(int width, int height) = 0;
	virtual HTEXTURE	CALL	Texture_Load(const char *filename, DWORD size=0, bool bMipmap=false) = 0;
	virtual void		CALL	Texture_LoadRawData( HTEXTURE _hTex, const char* data, int _pitch, int _width, int _height, int _format ) = 0;
	virtual void		CALL	Texture_Free(HTEXTURE tex) = 0;
	virtual int			CALL	Texture_GetWidth(HTEXTURE tex, bool bOriginal=false) = 0;
	virtual int			CALL	Texture_GetHeight(HTEXTURE tex, bool bOriginal=false) = 0;
	virtual DWORD*		CALL	Texture_Lock(HTEXTURE tex, int* _pitch, bool bReadOnly=true, int left=0, int top=0, int width=0, int height=0) = 0;
	virtual void		CALL	Texture_Unlock(HTEXTURE tex) = 0;
};

extern "C" { EXPORT HGE * CALL hgeCreate(int ver); }


/*
** HGE Virtual-key codes
*/
/*#define HGEK_LBUTTON	0x01
#define HGEK_RBUTTON	0x02
#define HGEK_MBUTTON	0x04

#define HGEK_ESCAPE		0x1B
#define HGEK_BACKSPACE	0x08
#define HGEK_TAB		0x09
#define HGEK_ENTER		0x0D
#define HGEK_SPACE		0x20

#define HGEK_SHIFT		0x10
#define HGEK_CTRL		0x11
#define HGEK_ALT		0x12

#define HGEK_LWIN		0x5B
#define HGEK_RWIN		0x5C
#define HGEK_APPS		0x5D

#define HGEK_PAUSE		0x13
#define HGEK_CAPSLOCK	0x14
#define HGEK_NUMLOCK	0x90
#define HGEK_SCROLLLOCK	0x91

#define HGEK_PGUP		0x21
#define HGEK_PGDN		0x22
#define HGEK_HOME		0x24
#define HGEK_END		0x23
#define HGEK_INSERT		0x2D
#define HGEK_DELETE		0x2E

#define HGEK_LEFT		0x25
#define HGEK_UP			0x26
#define HGEK_RIGHT		0x27
#define HGEK_DOWN		0x28

#define HGEK_0			0x30
#define HGEK_1			0x31
#define HGEK_2			0x32
#define HGEK_3			0x33
#define HGEK_4			0x34
#define HGEK_5			0x35
#define HGEK_6			0x36
#define HGEK_7			0x37
#define HGEK_8			0x38
#define HGEK_9			0x39

#define HGEK_A			0x41
#define HGEK_B			0x42
#define HGEK_C			0x43
#define HGEK_D			0x44
#define HGEK_E			0x45
#define HGEK_F			0x46
#define HGEK_G			0x47
#define HGEK_H			0x48
#define HGEK_I			0x49
#define HGEK_J			0x4A
#define HGEK_K			0x4B
#define HGEK_L			0x4C
#define HGEK_M			0x4D
#define HGEK_N			0x4E
#define HGEK_O			0x4F
#define HGEK_P			0x50
#define HGEK_Q			0x51
#define HGEK_R			0x52
#define HGEK_S			0x53
#define HGEK_T			0x54
#define HGEK_U			0x55
#define HGEK_V			0x56
#define HGEK_W			0x57
#define HGEK_X			0x58
#define HGEK_Y			0x59
#define HGEK_Z			0x5A

#define HGEK_GRAVE		0xC0
#define HGEK_MINUS		0xBD
#define HGEK_EQUALS		0xBB
#define HGEK_BACKSLASH	0xDC
#define HGEK_LBRACKET	0xDB
#define HGEK_RBRACKET	0xDD
#define HGEK_SEMICOLON	0xBA
#define HGEK_APOSTROPHE	0xDE
#define HGEK_COMMA		0xBC
#define HGEK_PERIOD		0xBE
#define HGEK_SLASH		0xBF

#define HGEK_NUMPAD0	0x60
#define HGEK_NUMPAD1	0x61
#define HGEK_NUMPAD2	0x62
#define HGEK_NUMPAD3	0x63
#define HGEK_NUMPAD4	0x64
#define HGEK_NUMPAD5	0x65
#define HGEK_NUMPAD6	0x66
#define HGEK_NUMPAD7	0x67
#define HGEK_NUMPAD8	0x68
#define HGEK_NUMPAD9	0x69

#define HGEK_MULTIPLY	0x6A
#define HGEK_DIVIDE		0x6F
#define HGEK_ADD		0x6B
#define HGEK_SUBTRACT	0x6D
#define HGEK_DECIMAL	0x6E

#define HGEK_F1			0x70
#define HGEK_F2			0x71
#define HGEK_F3			0x72
#define HGEK_F4			0x73
#define HGEK_F5			0x74
#define HGEK_F6			0x75
#define HGEK_F7			0x76
#define HGEK_F8			0x77
#define HGEK_F9			0x78
#define HGEK_F10		0x79
#define HGEK_F11		0x7A
#define HGEK_F12		0x7B
*/

#endif

