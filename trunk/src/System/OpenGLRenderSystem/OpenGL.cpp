#	include "OpenGL.h"

#	pragma comment(lib, "opengl32.lib")
#	pragma comment(lib, "glu32.lib")

PFNGLACTIVETEXTUREARBPROC glActiveTexture;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture;
PFNGLCLEARDEPTHFPROC glClearDepthf;

namespace Menge
{
	void initialize_GLEXT()
	{
		glActiveTexture = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress( "glActiveTexture" );
		glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)wglGetProcAddress( "glCompressedTexImage2D" );
		glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress( "glGenBuffers" );
		glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress( "glBindBuffer" );
		glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress( "glBufferData" );
		glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress( "glDeleteBuffers" );
		glBufferSubData = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress( "glBufferSubData" );
		glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC)wglGetProcAddress( "glClientActiveTexture" );
		glClearDepthf = (PFNGLCLEARDEPTHFPROC)wglGetProcAddress( "glClearDepthf" );
	}
}