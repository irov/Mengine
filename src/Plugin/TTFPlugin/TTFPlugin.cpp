#include "TTFPlugin.h"

#include "Interface/StringizeInterface.h"
#include "Interface/PrototypeManagerInterface.h"
#include "Interface/StringizeInterface.h"

#include "Kernel/PixelFormat.h"

#include "Kernel/Logger.h"

#include "TTFPrototypeGenerator.h"

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
	bool TTFPlugin::_initialize()
	{
		SERVICE_CREATE( TTFAtlasService );

		FT_Library ftlibrary;
		FT_Error ft_err = FT_Init_FreeType( &ftlibrary );

		if( ft_err != 0 )
		{
			LOGGER_ERROR("TTFPlugin::_initialize invalid init FreeType '%d'"
				, ft_err
				);

			return false;
		}

		TTFPrototypeGeneratorPtr generator = new FactorableUnique<TTFPrototypeGenerator>();

		generator->setFTLibrary( ftlibrary );

		PROTOTYPE_SERVICE()
			->addPrototype( STRINGIZE_STRING_LOCAL( "Font" ), STRINGIZE_STRING_LOCAL( "TTF" )
				, generator
			);

		m_ftlibrary = ftlibrary;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TTFPlugin::_finalize()
	{
		PROTOTYPE_SERVICE()
			->removePrototype( STRINGIZE_STRING_LOCAL( "Font" ), STRINGIZE_STRING_LOCAL( "TTF" ) );

		SERVICE_FINALIZE( TTFAtlasServiceInterface );

		FT_Done_FreeType( m_ftlibrary );
		m_ftlibrary = nullptr;
	}
}
