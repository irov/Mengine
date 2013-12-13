#	include "DevelopmentConverterPlugin.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "HotspotImageConverterPNGToHIT.h"
#	include "ParticleConverterPTCToPTZ.h"
#	include "SoundConverterFFMPEGToOGG.h"
#	include "VideoConverterFFMPEGToWEBM.h"
#	include "MovieKeyConverterXMLToAEK.h"

#   include "Logger/Logger.h"

#	include "Core/File.h"
#   include "Codec/ConverterFactory.h"

extern "C" // only required if using g++
{
	//////////////////////////////////////////////////////////////////////////
    bool initPluginDevelopmentConverter( Menge::PluginInterface ** _plugin )
    {
        *_plugin = new Menge::DevelopmentConverterPlugin();

        return true;
    }
    ////////////////////////////////////////////////////////////////////////////
#ifdef MENGE_COMPILER_MSVC
	__declspec(dllexport) 
#endif
	bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
    {
        return initPluginDevelopmentConverter( _plugin );
    }
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DevelopmentConverterPlugin::DevelopmentConverterPlugin()
        : m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool DevelopmentConverterPlugin::initialize( ServiceProviderInterface * _serviceProvider )
	{
        m_serviceProvider = _serviceProvider;        
       
		m_converters.push_back( new ConverterFactory<HotspotImageConverterPNGToHIT>(m_serviceProvider, Helper::stringizeString(m_serviceProvider, "png2hit")) );
		m_converters.push_back( new ConverterFactory<SoundConverterFFMPEGToOGG>(m_serviceProvider, Helper::stringizeString(m_serviceProvider, "ffmpegToOggSound")) );
		m_converters.push_back( new ConverterFactory<VideoConverterFFMPEGToWEBM>(m_serviceProvider, Helper::stringizeString(m_serviceProvider, "ffmpegToWebM")) );
		m_converters.push_back( new ConverterFactory<MovieKeyConverterXMLToAEK>(m_serviceProvider, Helper::stringizeString(m_serviceProvider, "xmlToAekMovie")) );
		
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
	void DevelopmentConverterPlugin::destroy()
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
		}

		delete this;
	}
}