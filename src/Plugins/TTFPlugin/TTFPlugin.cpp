#include "TTFPlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/DataServiceInterface.h"

#include "TTFFontPrototypeGenerator.h"
#include "TTFFontGlyphPrototypeGenerator.h"
#include "TTFFontConfigLoader.h"
#include "TTFFontGlyphConfigLoader.h"
#include "TTFDataflow.h"
#include "TTFFontValidator.h"
#include "TTFFontGlyphValidator.h"

#include "Kernel/Logger.h"
#include "Kernel/PixelFormat.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AllocatorHelper.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
void * _ft_malloc( size_t _size )
{
    void * p = Mengine::Helper::allocateMemory( _size, "ft" );

    return p;
}
//////////////////////////////////////////////////////////////////////////
void * _ft_calloc( size_t _count, size_t _size )
{
    void * p = Mengine::Helper::callocateMemory( _count, _size, "ft" );

    return p;
}
//////////////////////////////////////////////////////////////////////////
void _ft_free( void * _ptr )
{
    Mengine::Helper::deallocateMemory( _ptr, "ft" );
}
//////////////////////////////////////////////////////////////////////////
void * _ft_realloc( void * _ptr, size_t _size )
{
    void * p = Mengine::Helper::reallocateMemory( _ptr, _size, "ft" );

    return p;
}
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( TTFAtlasService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( TTF, Mengine::TTFPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TTFPlugin::TTFPlugin()
        : m_ftlibrary( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TTFPlugin::~TTFPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "nottf" ) == true )
        {
            return false;
        }

        bool TTFPlugin_Available = CONFIG_VALUE_BOOLEAN( "TTFPlugin", "Available", true );

        if( TTFPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFPlugin::_initializePlugin()
    {
        FT_Library ftlibrary;
        FT_Error ft_err = FT_Init_FreeType( &ftlibrary );

        if( ft_err != FT_Err_Ok )
        {
            LOGGER_ERROR( "invalid init FreeType '%d'"
                , ft_err
            );

            return false;
        }

        FT_Int amajor;
        FT_Int aminor;
        FT_Int apatch;
        FT_Library_Version( ftlibrary, &amajor, &aminor, &apatch );

        LOGGER_MESSAGE( "TTF version: %d.%d.%d"
            , amajor
            , aminor
            , apatch
        );

        m_ftlibrary = ftlibrary;

        if( SERVICE_CREATE( TTFAtlasService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        TTFFontPrototypeGeneratorPtr generator = Helper::makeFactorableUnique<TTFFontPrototypeGenerator>( MENGINE_DOCUMENT_FACTORABLE );

        generator->setFTLibrary( m_ftlibrary );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Font" ), STRINGIZE_STRING_LOCAL( "TTF" ), generator ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "FontGlyph" ), STRINGIZE_STRING_LOCAL( "TTF" ), Helper::makeFactorableUnique<TTFFontGlyphPrototypeGenerator>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        VOCABULARY_SET( ConfigLoaderInterface, STRINGIZE_STRING_LOCAL( "FontConfigLoader" ), STRINGIZE_STRING_LOCAL( "TTF" ), Helper::makeFactorableUnique<TTFFontConfigLoader>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ConfigLoaderInterface, STRINGIZE_STRING_LOCAL( "FontGlyphConfigLoader" ), STRINGIZE_STRING_LOCAL( "TTF" ), Helper::makeFactorableUnique<TTFFontGlyphConfigLoader>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "FontValidator" ), STRINGIZE_STRING_LOCAL( "TTFFont" ), Helper::makeFactorableUnique<TTFFontValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "FontGlyphValidator" ), STRINGIZE_STRING_LOCAL( "TTFFont" ), Helper::makeFactorableUnique<TTFFontGlyphValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        m_ftMutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        PLUGIN_SERVICE_WAIT( DataServiceInterface, [this]()
        {
            TTFDataflowPtr dataflowTTF = Helper::makeFactorableUnique<TTFDataflow>( MENGINE_DOCUMENT_FACTORABLE );

            dataflowTTF->setFTLibrary( m_ftlibrary );

            dataflowTTF->setMutex( m_ftMutex );

            if( dataflowTTF->initialize() == false )
            {
                return false;
            }

            VOCABULARY_SET( DataflowInterface, STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "ttfFont" ), dataflowTTF, MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( DataServiceInterface, []()
        {
            DataflowInterfacePtr dataflowTTF = VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "ttfFont" ) );
            dataflowTTF->finalize();
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFPlugin::_finalizePlugin()
    {
        m_ftMutex = nullptr;

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Font" ), STRINGIZE_STRING_LOCAL( "TTF" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "FontGlyph" ), STRINGIZE_STRING_LOCAL( "TTF" ), nullptr );

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FontConfigLoader" ), STRINGIZE_STRING_LOCAL( "TTF" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FontGlyphConfigLoader" ), STRINGIZE_STRING_LOCAL( "TTF" ) );

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FontValidator" ), STRINGIZE_STRING_LOCAL( "TTFFont" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FontGlyphValidator" ), STRINGIZE_STRING_LOCAL( "TTFFont" ) );

        SERVICE_FINALIZE( TTFAtlasService );

        FT_Done_FreeType( m_ftlibrary );
        m_ftlibrary = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( TTFAtlasService );

        MENGINE_ASSERTION_ALLOCATOR( "ft" );
    }
    //////////////////////////////////////////////////////////////////////////
}
