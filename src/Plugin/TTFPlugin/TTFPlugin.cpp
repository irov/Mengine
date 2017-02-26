#	include "TTFPlugin.h"

#	include "Interface/StringizeInterface.h"
#   include "Interface/PrototypeManagerInterface.h"

#   include "TTFFont.h"
#   include "TTFText.h"

#   include "Kernel/NodePrototypeGenerator.h"
#   include "Kernel/ScriptWrapper.h"

#	include "Core/PixelFormat.h"

#	include "Logger/Logger.h"

#   include "pybind/pybind.hpp"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( TTFAtlasService );
SERVICE_EXTERN( TTFFontManager );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( TTF, Menge::TTFPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TTFPlugin::TTFPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool TTFPlugin::_initialize()
	{
		SERVICE_CREATE( m_serviceProvider, TTFAtlasService );
		SERVICE_CREATE( m_serviceProvider, TTFFontManager );
			
        SCRIPT_SERVICE( m_serviceProvider )
            ->setWrapper( Helper::stringizeString( m_serviceProvider, "TTFText" ), new ScriptWrapper<TTFText>() );

        if( PROTOTYPE_SERVICE( m_serviceProvider )
            ->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Node" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "TTFText" ), new NodePrototypeGenerator<TTFText, 32> ) == false )
        {
            return false;
        }

        pybind::kernel_interface * kernel = pybind::get_kernel();

        pybind::interface_<TTFFontInterface>( kernel, "TTFFontInterface", false )
            .def_smart_pointer()            
            ;

        pybind::interface_<TTFText, pybind::bases<Node> >( kernel, "TTFText", false )
            .def( "setTextID", &TTFText::setTextID )
            .def( "getTextID", &TTFText::getTextID )
            .def( "setFontName", &TTFText::setFontName )
            .def( "getFontName", &TTFText::getFontName )
            ;

        pybind::def_functor( kernel, "createTTFFont", this, &TTFPlugin::createTTFFont );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TTFPlugin::_finalize()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool TTFPlugin::createTTFFont( const ConstString & _name, const ConstString& _pakName, const FilePath& _fileName )
    {
        bool successful = TTFFONT_SERVICE( m_serviceProvider )
            ->createFont( _name, _pakName, _fileName );

        return successful;
    }
}
