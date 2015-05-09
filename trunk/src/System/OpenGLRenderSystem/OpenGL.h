#	pragma once

#	include "SDL_opengl.h"

#   define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG                      0x8C00
#   define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG                      0x8C01
#   define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG                     0x8C02
#   define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG                     0x8C03

#   define GL_ETC1_RGB8_OES                                        0x8D64

extern PFNGLACTIVETEXTUREARBPROC glActiveTexture;
extern PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
extern PFNGLBUFFERSUBDATAPROC glBufferSubData;
extern PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture;
extern PFNGLCLEARDEPTHFPROC glClearDepthf;

namespace Menge
{
	void initialize_GLEXT();
}
