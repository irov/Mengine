#include "ResourceGenerators.h"

#include "Interface/ResourceServiceInterface.h"

#include "Kernel/ConstStringHelper.h"

#include "ResourceCursor.h"
#include "ResourceCursorICO.h"
#include "ResourceCursorSystem.h"
#include "ResourceFile.h"
#include "ResourceHIT.h"
#include "ResourceImageData.h"
#include "ResourceImageDefault.h"
#include "ResourceImageSolid.h"
#include "ResourceImageSubstract.h"
#include "ResourceImageSubstractRGBAndAlpha.h"
#include "ResourceImageSequence.h"
#include "ResourceMusic.h"
#include "ResourceShape.h"
#include "ResourceSound.h"
#include "ResourceTestPick.h"
#include "ResourceWindow.h"


//////////////////////////////////////////////////////////////////////////
#define MENGINE_RESOURCE_GENERATE(X)\
namespace Mengine::Helper{\
        X ## Ptr generate ## X( const Char * _doc ){\
            X ## Ptr text = RESOURCE_SERVICE()->generateResource( STRINGIZE_STRING_LOCAL( #X ), _doc );\
            return text;\
        }}
//////////////////////////////////////////////////////////////////////////
MENGINE_RESOURCE_GENERATE( ResourceCursor );
MENGINE_RESOURCE_GENERATE( ResourceCursorICO );
MENGINE_RESOURCE_GENERATE( ResourceCursorSystem );
MENGINE_RESOURCE_GENERATE( ResourceFile );
MENGINE_RESOURCE_GENERATE( ResourceHIT );
MENGINE_RESOURCE_GENERATE( ResourceImageData );
MENGINE_RESOURCE_GENERATE( ResourceImageDefault );
MENGINE_RESOURCE_GENERATE( ResourceImageSolid );
MENGINE_RESOURCE_GENERATE( ResourceImageSubstract );
MENGINE_RESOURCE_GENERATE( ResourceImageSubstractRGBAndAlpha );
MENGINE_RESOURCE_GENERATE( ResourceImageSequence );
MENGINE_RESOURCE_GENERATE( ResourceMusic );
MENGINE_RESOURCE_GENERATE( ResourceShape );
MENGINE_RESOURCE_GENERATE( ResourceSound );
MENGINE_RESOURCE_GENERATE( ResourceTestPick );
MENGINE_RESOURCE_GENERATE( ResourceWindow );
//////////////////////////////////////////////////////////////////////////
#undef MENGINE_NODE_GENERATE
//////////////////////////////////////////////////////////////////////////