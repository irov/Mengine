#include "MetabufLoaderPlugin.h"

#include "Interface/LoaderServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "LoaderResourceCursorICO.h"
#include "LoaderResourceCursorSystem.h"
#include "LoaderResourceFile.h"
#include "LoaderResourceHIT.h"
#include "LoaderResourceImageData.h"
#include "LoaderResourceImageDefault.h"
#include "LoaderResourceImageEmpty.h"
#include "LoaderResourceImageSequence.h"
#include "LoaderResourceImageSolid.h"
#include "LoaderResourceImageSubstract.h"
#include "LoaderResourceImageSubstractRGBAndAlpha.h"
#include "LoaderResourceImageSubstractRGBAndAlpha.h"
#include "LoaderResourceMusic.h"
#include "LoaderResourceShape.h"
#include "LoaderResourceSound.h"
#include "LoaderResourceWindow.h"

#include "Kernel/AllocatorHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"

//////////////////////////////////////////////////////////////////////////
void * _metabuf_malloc( size_t _size )
{
    void * p = Mengine::Helper::allocateMemory( _size, "metabuf" );

    return p;
}
//////////////////////////////////////////////////////////////////////////
void _metabuf_free( void * _ptr )
{
    Mengine::Helper::deallocateMemory( _ptr, "metabuf" );
}
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
    bool MetabufLoaderPlugin::_initializePlugin()
    {
#define SET_LOADER(T)\
        VOCABULARY_SET( LoaderInterface, STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( #T ), Helper::makeFactorableUnique<Loader##T>(MENGINE_DOCUMENT_FACTORABLE), MENGINE_DOCUMENT_FACTORABLE )

        SET_LOADER( ResourceCursorICO );
        SET_LOADER( ResourceCursorSystem );
        SET_LOADER( ResourceFile );
        SET_LOADER( ResourceHIT );
        SET_LOADER( ResourceImageData );
        SET_LOADER( ResourceImageDefault );
        SET_LOADER( ResourceImageEmpty );
        SET_LOADER( ResourceImageSequence );
        SET_LOADER( ResourceImageSolid );
        SET_LOADER( ResourceImageSubstract );
        SET_LOADER( ResourceImageSubstractRGBAndAlpha );
        SET_LOADER( ResourceMusic );
        SET_LOADER( ResourceShape );
        SET_LOADER( ResourceSound );
        SET_LOADER( ResourceWindow );

#undef SET_LOADER

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetabufLoaderPlugin::_finalizePlugin()
    {
#define REMOVE_LOADER(T)\
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( #T ) )

        REMOVE_LOADER( ResourceCursorICO );
        REMOVE_LOADER( ResourceCursorSystem );
        REMOVE_LOADER( ResourceFile );
        REMOVE_LOADER( ResourceHIT );
        REMOVE_LOADER( ResourceImageData );
        REMOVE_LOADER( ResourceImageDefault );
        REMOVE_LOADER( ResourceImageEmpty );
        REMOVE_LOADER( ResourceImageSequence );
        REMOVE_LOADER( ResourceImageSolid );
        REMOVE_LOADER( ResourceImageSubstract );
        REMOVE_LOADER( ResourceImageSubstractRGBAndAlpha );
        REMOVE_LOADER( ResourceMusic );
        REMOVE_LOADER( ResourceShape );
        REMOVE_LOADER( ResourceSound );
        REMOVE_LOADER( ResourceWindow );

#undef REMOVE_LOADER
    }
    //////////////////////////////////////////////////////////////////////////
}