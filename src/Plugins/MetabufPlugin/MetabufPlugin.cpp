#include "MetabufPlugin.h"

#include "Interface/LoaderServiceInterface.h"

#include "MetabufLoaderResourceCursorICO.h"
#include "MetabufLoaderResourceCursorSystem.h"
#include "MetabufLoaderResourceFile.h"
#include "MetabufLoaderResourceHIT.h"
#include "MetabufLoaderResourceImageData.h"
#include "MetabufLoaderResourceImageDefault.h"
#include "MetabufLoaderResourceImageEmpty.h"
#include "MetabufLoaderResourceImageSequence.h"
#include "MetabufLoaderResourceImageSolid.h"
#include "MetabufLoaderResourceImageSubstract.h"
#include "MetabufLoaderResourceImageSubstractRGBAndAlpha.h"
#include "MetabufLoaderResourceImageSubstractRGBAndAlpha.h"
#include "MetabufLoaderResourceMusic.h"
#include "MetabufLoaderResourceShape.h"
#include "MetabufLoaderResourceSound.h"
#include "MetabufLoaderResourceWindow.h"

#include "MetabufPackageLoader.h"

#include "Kernel/AllocatorHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PluginHelper.h"

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
PLUGIN_FACTORY( Metabuf, Mengine::MetabufPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetabufPlugin::MetabufPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetabufPlugin::~MetabufPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufPlugin::_initializePlugin()
    {
#define SET_LOADER(T)\
        VOCABULARY_SET( MetabufLoaderInterface, STRINGIZE_STRING_LOCAL( "MetabufLoader" ), STRINGIZE_STRING_LOCAL( #T ), Helper::makeFactorableUnique<MetabufLoader##T>(MENGINE_DOCUMENT_FACTORABLE), MENGINE_DOCUMENT_FACTORABLE )

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

        VOCABULARY_SET( PackageLoaderInterface, STRINGIZE_STRING_LOCAL( "PackageLoader" ), STRINGIZE_STRING_LOCAL( "xml" ), Helper::makeFactorableUnique<MetabufPackageLoader>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetabufPlugin::_finalizePlugin()
    {
#define REMOVE_LOADER(T)\
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "MetabufLoader" ), STRINGIZE_STRING_LOCAL( #T ) )

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

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "PackageLoader" ), STRINGIZE_STRING_LOCAL( "xml" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}