#pragma once

#include "Kernel/Surface.h"

//////////////////////////////////////////////////////////////////////////
#define MENGINE_SURFACE_GENERATE(X)\
    namespace Mengine{typedef IntrusiveSurfacePtr<class X> MENGINE_PP_CONCATENATE(X, Ptr);}\
    namespace Mengine::Helper{MENGINE_PP_CONCATENATE(X, Ptr) generate ## X( const DocumentInterfacePtr & _doc );}
//////////////////////////////////////////////////////////////////////////
MENGINE_SURFACE_GENERATE( SurfaceImage )
MENGINE_SURFACE_GENERATE( SurfaceImageSequence )
MENGINE_SURFACE_GENERATE( SurfaceSolidColor )
MENGINE_SURFACE_GENERATE( SurfaceSound )
MENGINE_SURFACE_GENERATE( SurfaceTrackMatte )
//////////////////////////////////////////////////////////////////////////
#undef MENGINE_SURFACE_GENERATE
//////////////////////////////////////////////////////////////////////////