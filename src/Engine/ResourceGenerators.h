#pragma once

#include "Kernel/Resource.h"

//////////////////////////////////////////////////////////////////////////
#define MENGINE_RESOURCE_GENERATE(X)\
namespace Mengine{typedef IntrusiveResourcePtr<class X> X ## Ptr;}\
namespace Mengine::Helper{X ## Ptr generate ## X( const Char * _doc );}
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
#undef MENGINE_RESOURCE_GENERATE
//////////////////////////////////////////////////////////////////////////