#include "OpenGLRenderExtension.h"

#if defined(MENGINE_RENDER_OPENGL_NORMAL) && defined(MENGINE_ENVIRONMENT_PLATFORM_MACOS)
namespace Mengine
{
    void initialize_GLEXT()
    {
        // Native macOS OpenGL exposes core symbols through the framework.
    }
}
#endif
