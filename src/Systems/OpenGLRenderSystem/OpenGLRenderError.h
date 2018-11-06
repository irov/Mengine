#pragma once

#include "Interface/ServiceProviderInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    bool openglRenderErrorCheck( const Char * _file, int _line );
}

#define OPENGL_RENDER_CHECK_ERROR() Mengine::openglRenderErrorCheck( __FILE__, __LINE__ )

#define GLCALL( Method, Args )\
    {\
        Method Args;\
        OPENGL_RENDER_CHECK_ERROR();\
    }

#define GLCALLR( R, Method, Args )\
    do{\
        R = Method Args;\
        OPENGL_RENDER_CHECK_ERROR();\
    }while(false);


#define IF_GLCALL( Method, Args )\
    Method Args;\
    if( OPENGL_RENDER_CHECK_ERROR() == true )
