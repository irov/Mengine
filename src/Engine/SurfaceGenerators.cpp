#include "SurfaceGenerators.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/ConstStringHelper.h"

#include "SurfaceImage.h"
#include "SurfaceImageSequence.h"
#include "SurfaceSolidColor.h"
#include "SurfaceSound.h"
#include "SurfaceTrackMatte.h"

//////////////////////////////////////////////////////////////////////////
#define MENGINE_SURFACE_GENERATE(X)\
namespace Mengine::Helper{\
        X ## Ptr generate ## X( const DocumentPtr & _doc ){\
            X ## Ptr text = PROTOTYPE_GENERATE( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( #X ), _doc );\
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