#include "OpenGLRenderExtension.h"

#if defined(MENGINE_RENDER_OPENGL_NORMAL)

#   include "SDL_video.h"

// textures
PFNGLACTIVETEXTUREPROC              glActiveTexture_ = nullptr;
PFNGLCLIENTACTIVETEXTUREPROC        glClientActiveTexture_ = nullptr;
PFNGLCOMPRESSEDTEXIMAGE2DPROC       glCompressedTexImage2D_ = nullptr;

// blending
PFNGLBLENDEQUATIONPROC              glBlendEquation_ = nullptr;
PFNGLBLENDFUNCSEPARATEPROC          glBlendFuncSeparate_ = nullptr;
PFNGLBLENDEQUATIONSEPARATEPROC      glBlendEquationSeparate_ = nullptr;

PFNGLGENVERTEXARRAYSPROC            glGenVertexArrays = nullptr;
PFNGLBINDVERTEXARRAYPROC            glBindVertexArray = nullptr;
PFNGLDELETEVERTEXARRAYSPROC         glDeleteVertexArrays = nullptr;

// vbo
PFNGLGENBUFFERSPROC                 glGenBuffers = nullptr;
PFNGLBINDBUFFERPROC                 glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC                 glBufferData = nullptr;
PFNGLMAPBUFFERPROC                  glMapBuffer = nullptr;
PFNGLUNMAPBUFFERPROC                glUnmapBuffer = nullptr;
PFNGLDELETEBUFFERSPROC              glDeleteBuffers = nullptr;
PFNGLBUFFERSUBDATAPROC              glBufferSubData = nullptr;

// framebuffer
PFNGLGENFRAMEBUFFERSPROC            glGenFramebuffers = nullptr;
PFNGLBINDFRAMEBUFFERPROC            glBindFramebuffer = nullptr;
PFNGLFRAMEBUFFERTEXTUREPROC         glFramebufferTexture = nullptr;
PFNGLDRAWBUFFERSPROC                glDrawBuffers = nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSPROC     glCheckFramebufferStatus = nullptr;
PFNGLDELETEFRAMEBUFFERSPROC         glDeleteFramebuffers = nullptr;

// shaders
PFNGLDELETEPROGRAMPROC              glDeleteProgram = nullptr;
PFNGLDELETESHADERPROC               glDeleteShader = nullptr;
PFNGLDETACHSHADERPROC               glDetachShader = nullptr;
PFNGLCREATESHADERPROC               glCreateShader = nullptr;
PFNGLSHADERSOURCEPROC               glShaderSource = nullptr;
PFNGLCOMPILESHADERPROC              glCompileShader = nullptr;
PFNGLCREATEPROGRAMPROC              glCreateProgram = nullptr;
PFNGLATTACHSHADERPROC               glAttachShader = nullptr;
PFNGLLINKPROGRAMPROC                glLinkProgram = nullptr;
PFNGLUSEPROGRAMPROC                 glUseProgram = nullptr;
PFNGLVALIDATEPROGRAMPROC            glValidateProgram = nullptr;
PFNGLGETPROGRAMIVPROC               glGetProgramiv = nullptr;
PFNGLGETSHADERIVPROC                glGetShaderiv = nullptr;
PFNGLGETPROGRAMINFOLOGPROC          glGetProgramInfoLog = nullptr;
PFNGLGETSHADERINFOLOGPROC           glGetShaderInfoLog = nullptr;
PFNGLUNIFORM1FPROC                  glUniform1f = nullptr;
PFNGLUNIFORM2FPROC                  glUniform2f = nullptr;
PFNGLUNIFORM3FPROC                  glUniform3f = nullptr;
PFNGLUNIFORM4FPROC                  glUniform4f = nullptr;
PFNGLUNIFORM1IPROC                  glUniform1i = nullptr;
PFNGLUNIFORM2IPROC                  glUniform2i = nullptr;
PFNGLUNIFORM3IPROC                  glUniform3i = nullptr;
PFNGLUNIFORM4IPROC                  glUniform4i = nullptr;
PFNGLUNIFORM1FVPROC                 glUniform1fv = nullptr;
PFNGLUNIFORM2FVPROC                 glUniform2fv = nullptr;
PFNGLUNIFORM3FVPROC                 glUniform3fv = nullptr;
PFNGLUNIFORM4FVPROC                 glUniform4fv = nullptr;
PFNGLUNIFORM1IVPROC                 glUniform1iv = nullptr;
PFNGLUNIFORM2IVPROC                 glUniform2iv = nullptr;
PFNGLUNIFORM3IVPROC                 glUniform3iv = nullptr;
PFNGLUNIFORM4IVPROC                 glUniform4iv = nullptr;
PFNGLUNIFORMMATRIX2FVPROC           glUniformMatrix2fv = nullptr;
PFNGLUNIFORMMATRIX3FVPROC           glUniformMatrix3fv = nullptr;
PFNGLUNIFORMMATRIX4FVPROC           glUniformMatrix4fv = nullptr;
PFNGLGETUNIFORMLOCATIONPROC         glGetUniformLocation = nullptr;
PFNGLGETACTIVEUNIFORMPROC           glGetActiveUniform = nullptr;
PFNGLGETUNIFORMFVPROC               glGetUniformfv = nullptr;
PFNGLGETUNIFORMIVPROC               glGetUniformiv = nullptr;
PFNGLGETSHADERSOURCEPROC            glGetShaderSource = nullptr;
PFNGLBINDATTRIBLOCATIONPROC         glBindAttribLocation = nullptr;
PFNGLGETACTIVEATTRIBPROC            glGetActiveAttrib = nullptr;
PFNGLGETATTRIBLOCATIONPROC          glGetAttribLocation = nullptr;
PFNGLVERTEXATTRIB1SPROC             glVertexAttrib1s = nullptr;
PFNGLVERTEXATTRIB1FPROC             glVertexAttrib1f = nullptr;
PFNGLVERTEXATTRIB1DPROC             glVertexAttrib1d = nullptr;
PFNGLVERTEXATTRIB2SPROC             glVertexAttrib2s = nullptr;
PFNGLVERTEXATTRIB2FPROC             glVertexAttrib2f = nullptr;
PFNGLVERTEXATTRIB2DPROC             glVertexAttrib2d = nullptr;
PFNGLVERTEXATTRIB3SPROC             glVertexAttrib3s = nullptr;
PFNGLVERTEXATTRIB3FPROC             glVertexAttrib3f = nullptr;
PFNGLVERTEXATTRIB3DPROC             glVertexAttrib3d = nullptr;
PFNGLVERTEXATTRIB4SPROC             glVertexAttrib4s = nullptr;
PFNGLVERTEXATTRIB4FPROC             glVertexAttrib4f = nullptr;
PFNGLVERTEXATTRIB4DPROC             glVertexAttrib4d = nullptr;
PFNGLVERTEXATTRIB4NUBPROC           glVertexAttrib4Nub = nullptr;
PFNGLVERTEXATTRIB1SVPROC            glVertexAttrib1sv = nullptr;
PFNGLVERTEXATTRIB1FVPROC            glVertexAttrib1fv = nullptr;
PFNGLVERTEXATTRIB1DVPROC            glVertexAttrib1dv = nullptr;
PFNGLVERTEXATTRIB2SVPROC            glVertexAttrib2sv = nullptr;
PFNGLVERTEXATTRIB2FVPROC            glVertexAttrib2fv = nullptr;
PFNGLVERTEXATTRIB2DVPROC            glVertexAttrib2dv = nullptr;
PFNGLVERTEXATTRIB3SVPROC            glVertexAttrib3sv = nullptr;
PFNGLVERTEXATTRIB3FVPROC            glVertexAttrib3fv = nullptr;
PFNGLVERTEXATTRIB3DVPROC            glVertexAttrib3dv = nullptr;
PFNGLVERTEXATTRIB4BVPROC            glVertexAttrib4bv = nullptr;
PFNGLVERTEXATTRIB4SVPROC            glVertexAttrib4sv = nullptr;
PFNGLVERTEXATTRIB4IVPROC            glVertexAttrib4iv = nullptr;
PFNGLVERTEXATTRIB4UBVPROC           glVertexAttrib4ubv = nullptr;
PFNGLVERTEXATTRIB4USVPROC           glVertexAttrib4usv = nullptr;
PFNGLVERTEXATTRIB4UIVPROC           glVertexAttrib4uiv = nullptr;
PFNGLVERTEXATTRIB4FVPROC            glVertexAttrib4fv = nullptr;
PFNGLVERTEXATTRIB4DVPROC            glVertexAttrib4dv = nullptr;
PFNGLVERTEXATTRIB4NBVPROC           glVertexAttrib4Nbv = nullptr;
PFNGLVERTEXATTRIB4NSVPROC           glVertexAttrib4Nsv = nullptr;
PFNGLVERTEXATTRIB4NIVPROC           glVertexAttrib4Niv = nullptr;
PFNGLVERTEXATTRIB4NUBVPROC          glVertexAttrib4Nubv = nullptr;
PFNGLVERTEXATTRIB4NUSVPROC          glVertexAttrib4Nusv = nullptr;
PFNGLVERTEXATTRIB4NUIVPROC          glVertexAttrib4Nuiv = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC        glVertexAttribPointer = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC    glEnableVertexAttribArray = nullptr;
PFNGLDISABLEVERTEXATTRIBARRAYPROC   glDisableVertexAttribArray = nullptr;
PFNGLGETVERTEXATTRIBDVPROC          glGetVertexAttribdv = nullptr;
PFNGLGETVERTEXATTRIBFVPROC          glGetVertexAttribfv = nullptr;
PFNGLGETVERTEXATTRIBIVPROC          glGetVertexAttribiv = nullptr;
PFNGLGETVERTEXATTRIBPOINTERVPROC    glGetVertexAttribPointerv = nullptr;

namespace Mengine
{
    void initialize_GLEXT()
    {
#   define GETGLPROC(ptype, pname) pname = reinterpret_cast<ptype>(SDL_GL_GetProcAddress(#pname))
#   define GETGLPROC_(ptype, pname) pname##_ = reinterpret_cast<ptype>(SDL_GL_GetProcAddress(#pname))

        if( nullptr == glGenBuffers || nullptr == glDeleteProgram )
        {
            // textures
            GETGLPROC_( PFNGLACTIVETEXTUREPROC, glActiveTexture );
            GETGLPROC_( PFNGLCLIENTACTIVETEXTUREPROC, glClientActiveTexture );
            GETGLPROC_( PFNGLCOMPRESSEDTEXIMAGE2DPROC, glCompressedTexImage2D );

            // blending
            GETGLPROC_( PFNGLBLENDEQUATIONPROC, glBlendEquation );
            GETGLPROC_( PFNGLBLENDFUNCSEPARATEPROC, glBlendFuncSeparate );
            GETGLPROC_( PFNGLBLENDEQUATIONSEPARATEPROC, glBlendEquationSeparate );

            GETGLPROC( PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays );
            GETGLPROC( PFNGLBINDVERTEXARRAYPROC, glBindVertexArray );
            GETGLPROC( PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays );

            // vbo
            GETGLPROC( PFNGLGENBUFFERSPROC, glGenBuffers );
            GETGLPROC( PFNGLBINDBUFFERPROC, glBindBuffer );
            GETGLPROC( PFNGLBUFFERDATAPROC, glBufferData );
            GETGLPROC( PFNGLMAPBUFFERPROC, glMapBuffer );
            GETGLPROC( PFNGLUNMAPBUFFERPROC, glUnmapBuffer );
            GETGLPROC( PFNGLDELETEBUFFERSPROC, glDeleteBuffers );
            GETGLPROC( PFNGLBUFFERSUBDATAPROC, glBufferSubData );

            // framebuffer
            GETGLPROC( PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers );
            GETGLPROC( PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer );
            GETGLPROC( PFNGLFRAMEBUFFERTEXTUREPROC, glFramebufferTexture );
            GETGLPROC( PFNGLDRAWBUFFERSPROC, glDrawBuffers );
            GETGLPROC( PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus );
            GETGLPROC( PFNGLDELETEFRAMEBUFFERSPROC, glDeleteFramebuffers );

            // shaders
            GETGLPROC( PFNGLDELETEPROGRAMPROC, glDeleteProgram );
            GETGLPROC( PFNGLDELETESHADERPROC, glDeleteShader );
            GETGLPROC( PFNGLDETACHSHADERPROC, glDetachShader );
            GETGLPROC( PFNGLCREATESHADERPROC, glCreateShader );
            GETGLPROC( PFNGLSHADERSOURCEPROC, glShaderSource );
            GETGLPROC( PFNGLCOMPILESHADERPROC, glCompileShader );
            GETGLPROC( PFNGLCREATEPROGRAMPROC, glCreateProgram );
            GETGLPROC( PFNGLATTACHSHADERPROC, glAttachShader );
            GETGLPROC( PFNGLLINKPROGRAMPROC, glLinkProgram );
            GETGLPROC( PFNGLUSEPROGRAMPROC, glUseProgram );
            GETGLPROC( PFNGLVALIDATEPROGRAMPROC, glValidateProgram );
            GETGLPROC( PFNGLGETPROGRAMIVPROC, glGetProgramiv );
            GETGLPROC( PFNGLGETSHADERIVPROC, glGetShaderiv );
            GETGLPROC( PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog );
            GETGLPROC( PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog );
            GETGLPROC( PFNGLUNIFORM1FPROC, glUniform1f );
            GETGLPROC( PFNGLUNIFORM2FPROC, glUniform2f );
            GETGLPROC( PFNGLUNIFORM3FPROC, glUniform3f );
            GETGLPROC( PFNGLUNIFORM4FPROC, glUniform4f );
            GETGLPROC( PFNGLUNIFORM1IPROC, glUniform1i );
            GETGLPROC( PFNGLUNIFORM2IPROC, glUniform2i );
            GETGLPROC( PFNGLUNIFORM3IPROC, glUniform3i );
            GETGLPROC( PFNGLUNIFORM4IPROC, glUniform4i );
            GETGLPROC( PFNGLUNIFORM1FVPROC, glUniform1fv );
            GETGLPROC( PFNGLUNIFORM2FVPROC, glUniform2fv );
            GETGLPROC( PFNGLUNIFORM3FVPROC, glUniform3fv );
            GETGLPROC( PFNGLUNIFORM4FVPROC, glUniform4fv );
            GETGLPROC( PFNGLUNIFORM1IVPROC, glUniform1iv );
            GETGLPROC( PFNGLUNIFORM2IVPROC, glUniform2iv );
            GETGLPROC( PFNGLUNIFORM3IVPROC, glUniform3iv );
            GETGLPROC( PFNGLUNIFORM4IVPROC, glUniform4iv );
            GETGLPROC( PFNGLUNIFORMMATRIX2FVPROC, glUniformMatrix2fv );
            GETGLPROC( PFNGLUNIFORMMATRIX3FVPROC, glUniformMatrix3fv );
            GETGLPROC( PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv );
            GETGLPROC( PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation );
            GETGLPROC( PFNGLGETACTIVEUNIFORMPROC, glGetActiveUniform );
            GETGLPROC( PFNGLGETUNIFORMFVPROC, glGetUniformfv );
            GETGLPROC( PFNGLGETUNIFORMIVPROC, glGetUniformiv );
            GETGLPROC( PFNGLGETSHADERSOURCEPROC, glGetShaderSource );
            GETGLPROC( PFNGLBINDATTRIBLOCATIONPROC, glBindAttribLocation );
            GETGLPROC( PFNGLGETACTIVEATTRIBPROC, glGetActiveAttrib );
            GETGLPROC( PFNGLGETATTRIBLOCATIONPROC, glGetAttribLocation );
            GETGLPROC( PFNGLVERTEXATTRIB1SPROC, glVertexAttrib1s );
            GETGLPROC( PFNGLVERTEXATTRIB1FPROC, glVertexAttrib1f );
            GETGLPROC( PFNGLVERTEXATTRIB1DPROC, glVertexAttrib1d );
            GETGLPROC( PFNGLVERTEXATTRIB2SPROC, glVertexAttrib2s );
            GETGLPROC( PFNGLVERTEXATTRIB2FPROC, glVertexAttrib2f );
            GETGLPROC( PFNGLVERTEXATTRIB2DPROC, glVertexAttrib2d );
            GETGLPROC( PFNGLVERTEXATTRIB3SPROC, glVertexAttrib3s );
            GETGLPROC( PFNGLVERTEXATTRIB3FPROC, glVertexAttrib3f );
            GETGLPROC( PFNGLVERTEXATTRIB3DPROC, glVertexAttrib3d );
            GETGLPROC( PFNGLVERTEXATTRIB4SPROC, glVertexAttrib4s );
            GETGLPROC( PFNGLVERTEXATTRIB4FPROC, glVertexAttrib4f );
            GETGLPROC( PFNGLVERTEXATTRIB4DPROC, glVertexAttrib4d );
            GETGLPROC( PFNGLVERTEXATTRIB4NUBPROC, glVertexAttrib4Nub );
            GETGLPROC( PFNGLVERTEXATTRIB1SVPROC, glVertexAttrib1sv );
            GETGLPROC( PFNGLVERTEXATTRIB1FVPROC, glVertexAttrib1fv );
            GETGLPROC( PFNGLVERTEXATTRIB1DVPROC, glVertexAttrib1dv );
            GETGLPROC( PFNGLVERTEXATTRIB2SVPROC, glVertexAttrib2sv );
            GETGLPROC( PFNGLVERTEXATTRIB2FVPROC, glVertexAttrib2fv );
            GETGLPROC( PFNGLVERTEXATTRIB2DVPROC, glVertexAttrib2dv );
            GETGLPROC( PFNGLVERTEXATTRIB3SVPROC, glVertexAttrib3sv );
            GETGLPROC( PFNGLVERTEXATTRIB3FVPROC, glVertexAttrib3fv );
            GETGLPROC( PFNGLVERTEXATTRIB3DVPROC, glVertexAttrib3dv );
            GETGLPROC( PFNGLVERTEXATTRIB4BVPROC, glVertexAttrib4bv );
            GETGLPROC( PFNGLVERTEXATTRIB4SVPROC, glVertexAttrib4sv );
            GETGLPROC( PFNGLVERTEXATTRIB4IVPROC, glVertexAttrib4iv );
            GETGLPROC( PFNGLVERTEXATTRIB4UBVPROC, glVertexAttrib4ubv );
            GETGLPROC( PFNGLVERTEXATTRIB4USVPROC, glVertexAttrib4usv );
            GETGLPROC( PFNGLVERTEXATTRIB4UIVPROC, glVertexAttrib4uiv );
            GETGLPROC( PFNGLVERTEXATTRIB4FVPROC, glVertexAttrib4fv );
            GETGLPROC( PFNGLVERTEXATTRIB4DVPROC, glVertexAttrib4dv );
            GETGLPROC( PFNGLVERTEXATTRIB4NBVPROC, glVertexAttrib4Nbv );
            GETGLPROC( PFNGLVERTEXATTRIB4NSVPROC, glVertexAttrib4Nsv );
            GETGLPROC( PFNGLVERTEXATTRIB4NIVPROC, glVertexAttrib4Niv );
            GETGLPROC( PFNGLVERTEXATTRIB4NUBVPROC, glVertexAttrib4Nubv );
            GETGLPROC( PFNGLVERTEXATTRIB4NUSVPROC, glVertexAttrib4Nusv );
            GETGLPROC( PFNGLVERTEXATTRIB4NUIVPROC, glVertexAttrib4Nuiv );
            GETGLPROC( PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer );
            GETGLPROC( PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray );
            GETGLPROC( PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray );
            GETGLPROC( PFNGLGETVERTEXATTRIBDVPROC, glGetVertexAttribdv );
            GETGLPROC( PFNGLGETVERTEXATTRIBFVPROC, glGetVertexAttribfv );
            GETGLPROC( PFNGLGETVERTEXATTRIBIVPROC, glGetVertexAttribiv );
            GETGLPROC( PFNGLGETVERTEXATTRIBPOINTERVPROC, glGetVertexAttribPointerv );
        }

#   undef GETGLPROC
#   undef GETGLPROC_
    }
}
#endif
