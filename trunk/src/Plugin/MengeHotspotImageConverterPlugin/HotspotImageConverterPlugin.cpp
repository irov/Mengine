#	include "HotspotImageConverterPlugin.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "HotspotImageConverterPNGToHIT.h"

#   include "Logger/Logger.h"

#	include "Core/File.h"
#   include "Codec/ConverterFactory.h"

extern "C" // only required if using g++
{
    //////////////////////////////////////////////////////////////////////////
    __declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
    {
        *_plugin = new Menge::HotspotImageConverterPlugin();

        return true;
    }
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	HotspotImageConverterPlugin::HotspotImageConverterPlugin()
        : m_serviceProvider(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotspotImageConverterPlugin::initialize( ServiceProviderInterface * _serviceProvider )
	{
        m_serviceProvider = _serviceProvider;        
       
		const ConstString & c_png2hit = Helper::stringizeString(m_serviceProvider, "png2hit");

		m_converters.push_back( new ConverterFactory<HotspotImageConverterPNGToHIT>(m_serviceProvider, c_png2hit) );
		
		for( TVectorHotspotImageConverters::iterator
			it = m_converters.begin(),
			it_end = m_converters.end();
		it != it_end;
		++it )
		{
            ConverterFactoryInterface * converter = (*it);

			const ConstString & name = converter->getName();

			CONVERTER_SERVICE(m_serviceProvider)
                ->registerConverter( name, converter );
		}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotspotImageConverterPlugin::destroy()
	{
		for( TVectorHotspotImageConverters::iterator
			it = m_converters.begin(),
			it_end = m_converters.end();
		it != it_end;
		++it )
		{
            ConverterFactoryInterface * converter = (*it);

			const ConstString & name = converter->getName();

			CONVERTER_SERVICE(m_serviceProvider)
                ->unregisterConverter( name );

			converter->destroy();
		}

		delete this;
	}
}