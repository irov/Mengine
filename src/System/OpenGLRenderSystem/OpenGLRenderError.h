#   pragma once

#   include "Interface/ServiceInterface.h"

namespace Menge
{
    bool openglRenderErrorCheck( ServiceProviderInterface * _serviceProvider, const char * _method, const char * _file, int _line );
}

#   define OPENGL_RENDER_CHECK_ERROR(serviceProvider, Method) Menge::openglRenderErrorCheck( serviceProvider, Method, __FILE__, __LINE__ )

#   define GLCALL( ServiceProvider, Method, Args )\
    {\
        Method Args;\
        OPENGL_RENDER_CHECK_ERROR(ServiceProvider, #Method);\
    }

#   define GLCALLR( ServiceProvider, R, Method, Args )\
    do{\
        R = Method Args;\
        OPENGL_RENDER_CHECK_ERROR(ServiceProvider, #Method);\
    }while(false);


#   define IF_GLCALL( ServiceProvider, Method, Args )\
    if( Method Args, OPENGL_RENDER_CHECK_ERROR(ServiceProvider, #Method) == true )
