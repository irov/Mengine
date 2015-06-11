#	include "OpenGL.h"

#	include "SDL_video.h"

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
		SDL_bool result = SDL_GL_ExtensionSupported( "glActiveTexture" );

		glActiveTexture = (PFNGLACTIVETEXTUREARBPROC)SDL_GL_GetProcAddress( "glActiveTexture" );
		glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)SDL_GL_GetProcAddress( "glCompressedTexImage2D" );
		glGenBuffers = (PFNGLGENBUFFERSPROC)SDL_GL_GetProcAddress( "glGenBuffers" );
		glBindBuffer = (PFNGLBINDBUFFERPROC)SDL_GL_GetProcAddress( "glBindBuffer" );
		glBufferData = (PFNGLBUFFERDATAPROC)SDL_GL_GetProcAddress( "glBufferData" );
		glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)SDL_GL_GetProcAddress( "glDeleteBuffers" );
		glBufferSubData = (PFNGLBUFFERSUBDATAPROC)SDL_GL_GetProcAddress( "glBufferSubData" );
		glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC)SDL_GL_GetProcAddress( "glClientActiveTexture" );
		glClearDepthf = (PFNGLCLEARDEPTHFPROC)SDL_GL_GetProcAddress( "glClearDepthf" );
	}
}