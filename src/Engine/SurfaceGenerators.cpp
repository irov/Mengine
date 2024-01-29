#include "SurfaceGenerators.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/PrototypeHelper.h"

#include "Kernel/SurfaceImage.h"
#include "Kernel/SurfaceImageSequence.h"
#include "Kernel/SurfaceSolidColor.h"
#include "Kernel/SurfaceSound.h"
#include "Kernel/SurfaceTrackMatte.h"

//////////////////////////////////////////////////////////////////////////
#define MENGINE_SURFACE_GENERATE(X)\
namespace Mengine::Helper{\
        X ## Ptr generate ## X( const DocumentInterfacePtr & _doc ){\
            X ## Ptr text = Helper::generateFactorable<Surface, X>( _doc );\
            return text;\
        }}
//////////////////////////////////////////////////////////////////////////
MENGINE_SURFACE_GENERATE( SurfaceImage )
MENGINE_SURFACE_GENERATE( SurfaceImageSequence )
MENGINE_SURFACE_GENERATE( SurfaceSolidColor )
MENGINE_SURFACE_GENERATE( SurfaceSound )
MENGINE_SURFACE_GENERATE( SurfaceTrackMatte )
//////////////////////////////////////////////////////////////////////////
#undef MENGINE_SURFACE_GENERATE
//////////////////////////////////////////////////////////////////////////