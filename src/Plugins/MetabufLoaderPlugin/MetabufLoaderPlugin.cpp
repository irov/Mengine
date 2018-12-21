#include "MetabufLoaderPlugin.h"

#include "Interface/LoaderServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "LoaderResourceCursorICO.h"
#include "LoaderResourceCursorSystem.h"
#include "LoaderResourceFile.h"
#include "LoaderResourceHIT.h"
#include "LoaderResourceImageData.h"
#include "LoaderResourceImageDefault.h"
#include "LoaderResourceImageSequence.h"
#include "LoaderResourceImageSolid.h"
#include "LoaderResourceImageSubstract.h"
#include "LoaderResourceImageSubstractRGBAndAlpha.h"
#include "LoaderResourceImageSubstractRGBAndAlpha.h"
#include "LoaderResourceMusic.h"
#include "LoaderResourceShape.h"
#include "LoaderResourceSound.h"
#include "LoaderResourceWindow.h"

#include "Kernel/FactorableUnique.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MetabufLoader, Mengine::MetabufLoaderPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderPlugin::MetabufLoaderPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderPlugin::~MetabufLoaderPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufLoaderPlugin::_initialize()
    {
#define DECLARE_LOADER(T)\
        VOCALUBARY_SET( LoaderInterface, STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( #T ), new FactorableUnique<Loader##T>() )

        DECLARE_LOADER( ResourceCursorICO );
        DECLARE_LOADER( ResourceCursorSystem );
        DECLARE_LOADER( ResourceFile );
        DECLARE_LOADER( ResourceHIT );
        DECLARE_LOADER( ResourceImageData );
        DECLARE_LOADER( ResourceImageDefault );
        DECLARE_LOADER( ResourceImageSequence );
        DECLARE_LOADER( ResourceImageSolid );
        DECLARE_LOADER( ResourceImageSubstract );
        DECLARE_LOADER( ResourceImageSubstractRGBAndAlpha );
        DECLARE_LOADER( ResourceImageSubstractRGBAndAlpha );
        DECLARE_LOADER( ResourceMusic );
        DECLARE_LOADER( ResourceShape );
        DECLARE_LOADER( ResourceSound );
        DECLARE_LOADER( ResourceWindow );

#undef DECLARE_LOADER

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetabufLoaderPlugin::_finalize()
    {
    }
}