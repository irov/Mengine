#pragma once

#include "Environment/OpenGL/OpenGLRenderIncluder.h"

#ifdef MENGINE_RENDER_OPENGL_NORMAL
// textures
typedef void (APIENTRYP PFNGLACTIVETEXTUREPROC) (GLenum texture);
typedef void (APIENTRYP PFNGLCLIENTACTIVETEXTUREPROC) (GLenum texture);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE2DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid * data);

extern PFNGLACTIVETEXTUREPROC               glActiveTexture_;
extern PFNGLCLIENTACTIVETEXTUREPROC         glClientActiveTexture_;
extern PFNGLCOMPRESSEDTEXIMAGE2DPROC        glCompressedTexImage2D_;

// blending
typedef void (APIENTRYP PFNGLBLENDEQUATIONPROC) (GLenum mode);

extern PFNGLBLENDEQUATIONPROC               glBlendEquation_;
extern PFNGLBLENDFUNCSEPARATEPROC           glBlendFuncSeparate_;
extern PFNGLBLENDEQUATIONSEPARATEPROC       glBlendEquationSeparate_;

// tt
extern PFNGLGENVERTEXARRAYSPROC             glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC             glBindVertexArray;
extern PFNGLDELETEVERTEXARRAYSPROC          glDeleteVertexArrays;

// vbo
extern PFNGLGENBUFFERSPROC                  glGenBuffers;
extern PFNGLBINDBUFFERPROC                  glBindBuffer;
extern PFNGLBUFFERDATAPROC                  glBufferData;
extern PFNGLMAPBUFFERPROC                   glMapBuffer;
extern PFNGLUNMAPBUFFERPROC                 glUnmapBuffer;
extern PFNGLDELETEBUFFERSPROC               glDeleteBuffers;
extern PFNGLBUFFERSUBDATAPROC               glBufferSubData;

extern PFNGLGENFRAMEBUFFERSPROC             glGenFramebuffers;
extern PFNGLBINDFRAMEBUFFERPROC             glBindFramebuffer;
extern PFNGLFRAMEBUFFERTEXTUREPROC          glFramebufferTexture;
extern PFNGLDRAWBUFFERSPROC                 glDrawBuffers;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC      glCheckFramebufferStatus;
extern PFNGLDELETEFRAMEBUFFERSPROC          glDeleteFramebuffers;

// shaders
extern PFNGLDELETEPROGRAMPROC               glDeleteProgram;
extern PFNGLDELETESHADERPROC                glDeleteShader;
extern PFNGLDETACHSHADERPROC                glDetachShader;
extern PFNGLCREATESHADERPROC                glCreateShader;
extern PFNGLSHADERSOURCEPROC                glShaderSource;
extern PFNGLCOMPILESHADERPROC               glCompileShader;
extern PFNGLCREATEPROGRAMPROC               glCreateProgram;
extern PFNGLATTACHSHADERPROC                glAttachShader;
extern PFNGLLINKPROGRAMPROC                 glLinkProgram;
extern PFNGLUSEPROGRAMPROC                  glUseProgram;
extern PFNGLVALIDATEPROGRAMPROC             glValidateProgram;
extern PFNGLGETPROGRAMIVPROC                glGetProgramiv;
extern PFNGLGETSHADERIVPROC                 glGetShaderiv;
extern PFNGLGETPROGRAMINFOLOGPROC           glGetProgramInfoLog;
extern PFNGLGETSHADERINFOLOGPROC            glGetShaderInfoLog;
extern PFNGLUNIFORM1FPROC                   glUniform1f;
extern PFNGLUNIFORM2FPROC                   glUniform2f;
extern PFNGLUNIFORM3FPROC                   glUniform3f;
extern PFNGLUNIFORM4FPROC                   glUniform4f;
extern PFNGLUNIFORM1IPROC                   glUniform1i;
extern PFNGLUNIFORM2IPROC                   glUniform2i;
extern PFNGLUNIFORM3IPROC                   glUniform3i;
extern PFNGLUNIFORM4IPROC                   glUniform4i;
extern PFNGLUNIFORM1FVPROC                  glUniform1fv;
extern PFNGLUNIFORM2FVPROC                  glUniform2fv;
extern PFNGLUNIFORM3FVPROC                  glUniform3fv;
extern PFNGLUNIFORM4FVPROC                  glUniform4fv;
extern PFNGLUNIFORM1IVPROC                  glUniform1iv;
extern PFNGLUNIFORM2IVPROC                  glUniform2iv;
extern PFNGLUNIFORM3IVPROC                  glUniform3iv;
extern PFNGLUNIFORM4IVPROC                  glUniform4iv;
extern PFNGLUNIFORMMATRIX2FVPROC            glUniformMatrix2fv;
extern PFNGLUNIFORMMATRIX3FVPROC            glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC            glUniformMatrix4fv;
extern PFNGLGETUNIFORMLOCATIONPROC          glGetUniformLocation;
extern PFNGLGETACTIVEUNIFORMPROC            glGetActiveUniform;
extern PFNGLGETUNIFORMFVPROC                glGetUniformfv;
extern PFNGLGETUNIFORMIVPROC                glGetUniformiv;
extern PFNGLGETSHADERSOURCEPROC             glGetShaderSource;
extern PFNGLBINDATTRIBLOCATIONPROC          glBindAttribLocation;
extern PFNGLGETACTIVEATTRIBPROC             glGetActiveAttrib;
extern PFNGLGETATTRIBLOCATIONPROC           glGetAttribLocation;
extern PFNGLVERTEXATTRIB1SPROC              glVertexAttrib1s;
extern PFNGLVERTEXATTRIB1FPROC              glVertexAttrib1f;
extern PFNGLVERTEXATTRIB1DPROC              glVertexAttrib1d;
extern PFNGLVERTEXATTRIB2SPROC              glVertexAttrib2s;
extern PFNGLVERTEXATTRIB2FPROC              glVertexAttrib2f;
extern PFNGLVERTEXATTRIB2DPROC              glVertexAttrib2d;
extern PFNGLVERTEXATTRIB3SPROC              glVertexAttrib3s;
extern PFNGLVERTEXATTRIB3FPROC              glVertexAttrib3f;
extern PFNGLVERTEXATTRIB3DPROC              glVertexAttrib3d;
extern PFNGLVERTEXATTRIB4SPROC              glVertexAttrib4s;
extern PFNGLVERTEXATTRIB4FPROC              glVertexAttrib4f;
extern PFNGLVERTEXATTRIB4DPROC              glVertexAttrib4d;
extern PFNGLVERTEXATTRIB4NUBPROC            glVertexAttrib4Nub;
extern PFNGLVERTEXATTRIB1SVPROC             glVertexAttrib1sv;
extern PFNGLVERTEXATTRIB1FVPROC             glVertexAttrib1fv;
extern PFNGLVERTEXATTRIB1DVPROC             glVertexAttrib1dv;
extern PFNGLVERTEXATTRIB2SVPROC             glVertexAttrib2sv;
extern PFNGLVERTEXATTRIB2FVPROC             glVertexAttrib2fv;
extern PFNGLVERTEXATTRIB2DVPROC             glVertexAttrib2dv;
extern PFNGLVERTEXATTRIB3SVPROC             glVertexAttrib3sv;
extern PFNGLVERTEXATTRIB3FVPROC             glVertexAttrib3fv;
extern PFNGLVERTEXATTRIB3DVPROC             glVertexAttrib3dv;
extern PFNGLVERTEXATTRIB4BVPROC             glVertexAttrib4bv;
extern PFNGLVERTEXATTRIB4SVPROC             glVertexAttrib4sv;
extern PFNGLVERTEXATTRIB4IVPROC             glVertexAttrib4iv;
extern PFNGLVERTEXATTRIB4UBVPROC            glVertexAttrib4ubv;
extern PFNGLVERTEXATTRIB4USVPROC            glVertexAttrib4usv;
extern PFNGLVERTEXATTRIB4UIVPROC            glVertexAttrib4uiv;
extern PFNGLVERTEXATTRIB4FVPROC             glVertexAttrib4fv;
extern PFNGLVERTEXATTRIB4DVPROC             glVertexAttrib4dv;
extern PFNGLVERTEXATTRIB4NBVPROC            glVertexAttrib4Nbv;
extern PFNGLVERTEXATTRIB4NSVPROC            glVertexAttrib4Nsv;
extern PFNGLVERTEXATTRIB4NIVPROC            glVertexAttrib4Niv;
extern PFNGLVERTEXATTRIB4NUBVPROC           glVertexAttrib4Nubv;
extern PFNGLVERTEXATTRIB4NUSVPROC           glVertexAttrib4Nusv;
extern PFNGLVERTEXATTRIB4NUIVPROC           glVertexAttrib4Nuiv;
extern PFNGLVERTEXATTRIBPOINTERPROC         glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC     glEnableVertexAttribArray;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC    glDisableVertexAttribArray;
extern PFNGLGETVERTEXATTRIBDVPROC           glGetVertexAttribdv;
extern PFNGLGETVERTEXATTRIBFVPROC           glGetVertexAttribfv;
extern PFNGLGETVERTEXATTRIBIVPROC           glGetVertexAttribiv;
extern PFNGLGETVERTEXATTRIBPOINTERVPROC     glGetVertexAttribPointerv;

namespace Mengine
{
    void initialize_GLEXT();
}
#endif

