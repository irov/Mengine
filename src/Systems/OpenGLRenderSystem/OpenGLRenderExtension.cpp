#include "OpenGLRenderExtension.h"

#if defined(MENGINE_RENDER_OPENGL_NORMAL)

#if defined(MENGINE_ENVIRONMENT_PLATFORM_UNIX)
#include "Interface/PlatformServiceInterface.h"

#include "Environment/Unix/UnixPlatformServiceExtensionInterface.h"
#endif

namespace Mengine
{
    bool initialize_GLEXT()
    {
#if defined(MENGINE_ENVIRONMENT_PLATFORM_MACOS)
        // Native macOS OpenGL exposes core symbols through the framework.
        return true;
#elif defined(MENGINE_ENVIRONMENT_PLATFORM_UNIX)
        UnixPlatformServiceExtensionInterface * unixPlatform = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        if( unixPlatform == nullptr )
        {
            return false;
        }

        int gladVersion = gladLoadGL( []( const char * _name ) -> GLADapiproc
        {
            UnixPlatformServiceExtensionInterface * platform = PLATFORM_SERVICE()
                ->getDynamicUnknown();

            if( platform == nullptr )
            {
                return nullptr;
            }

            UnixOpenGLProc proc = platform->getOpenGLProcAddress( _name );

            return reinterpret_cast<GLADapiproc>(proc);
        } );

        if( gladVersion == 0 || GLAD_GL_VERSION_3_3 == 0 )
        {
            return false;
        }

        return true;
#else
#   error "Unsupported desktop OpenGL platform"
#endif
    }

#if defined(MENGINE_RENDER_OPENGL_NORMAL_LINUX)
    bool isGLTextureCompressionS3TCSupported()
    {
        if( GLAD_GL_EXT_texture_compression_s3tc == 0 )
        {
            return false;
        }

        return true;
    }
#endif
}
#endif
