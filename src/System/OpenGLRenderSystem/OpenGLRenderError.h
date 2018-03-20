#   pragma once

#   include "Interface/ServiceInterface.h"

namespace Menge
{
    bool openglRenderErrorCheck( const char * _method, const char * _file, int _line );
}

#   define OPENGL_RENDER_CHECK_ERROR(Method) Menge::openglRenderErrorCheck( Method, __FILE__, __LINE__ )

#   define GLCALL( Method, Args )\
    {\
        Method Args;\
        OPENGL_RENDER_CHECK_ERROR(#Method);\
    }

#   define GLCALLR( R, Method, Args )\
    do{\
        R = Method Args;\
        OPENGL_RENDER_CHECK_ERROR(#Method);\
    }while(false);


#   define IF_GLCALL( Method, Args )\
    if( Method Args, OPENGL_RENDER_CHECK_ERROR(#Method) == true )
