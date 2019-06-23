#include "TTFPlugin.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/PixelFormat.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#include "TTFPrototypeGenerator.h"
#include "TTFDataflow.h"
#include "FEDataflow.h"

#include "fe/fe.h"

//////////////////////////////////////////////////////////////////////////
void* _fe_alloc( size_t size )
{
    return stdex_malloc( size, "fe" );
}
//////////////////////////////////////////////////////////////////////////
void _fe_free( void *ptr )
{
    stdex_free( ptr, "fe" );
}
//////////////////////////////////////////////////////////////////////////
void _debug_image_created( fe_image * )
{}
//////////////////////////////////////////////////////////////////////////
void _debug_image_deleted( fe_image * )
{}
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
    bool TTFPlugin::_initializePlugin()
    {
        SERVICE_CREATE( TTFAtlasService );

        FT_Library ftlibrary;
        FT_Error ft_err = FT_Init_FreeType( &ftlibrary );

        if( ft_err != 0 )
        {
            LOGGER_ERROR( "invalid init FreeType '%d'"
                , ft_err
            );

            return false;
        }

        TTFPrototypeGeneratorPtr generator = Helper::makeFactorableUnique<TTFPrototypeGenerator>();

        generator->setFTLibrary( ftlibrary );

        PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Font" ), STRINGIZE_STRING_LOCAL( "TTF" )
                , generator
            );

        m_ftlibrary = ftlibrary;

        m_ftMutex = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FUNCTION );

        TTFDataflowPtr dataflowTTF = Helper::makeFactorableUnique<TTFDataflow>();

        dataflowTTF->setFTLibrary( ftlibrary );

        dataflowTTF->setMutex( m_ftMutex );

        if( dataflowTTF->initialize() == false )
        {
            return false;
        }

        VOCABULARY_SET( DataflowInterface, STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "ttfFont" ), dataflowTTF );

        FEDataflowPtr dataflowFE = Helper::makeFactorableUnique<FEDataflow>();

        if( dataflowFE->initialize() == false )
        {
            return false;
        }

        VOCABULARY_SET( DataflowInterface, STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "feFont" ), dataflowFE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFPlugin::_finalizePlugin()
    {
        m_ftMutex = nullptr;

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Font" ), STRINGIZE_STRING_LOCAL( "TTF" ) );

        DataflowInterfacePtr dataflowTTF = VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "ttfFont" ) );
        dataflowTTF->finalize();

        DataflowInterfacePtr dataflowFE = VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "feFont" ) );
        dataflowFE->finalize();

        SERVICE_FINALIZE( TTFAtlasServiceInterface );

        FT_Done_FreeType( m_ftlibrary );
        m_ftlibrary = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFPlugin::_destroy()
    {
        SERVICE_DESTROY( TTFAtlasServiceInterface );
    }
}
