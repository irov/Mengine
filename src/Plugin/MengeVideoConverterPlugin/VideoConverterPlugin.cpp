#	include "VideoConverterPlugin.h"

#	include "VideoConverterFFMPEGToWEBM.h"

#	include "Core/File.h"
#   include "Codec/ConverterFactory.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/StringizeInterface.h"

//////////////////////////////////////////////////////////////////////////
__declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
{
	*_plugin = new Menge::VideoConverterPlugin();
	return true;
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	VideoConverterPlugin::VideoConverterPlugin()
        : m_serviceProvider(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoConverterPlugin::initialize( ServiceProviderInterface * _serviceProvider )
	{
        m_serviceProvider = _serviceProvider;

		m_converters.push_back( new ConverterFactory<VideoConverterFFMPEGToWEBM>(m_serviceProvider, Helper::stringizeString(m_serviceProvider, "ffmpegToWebM")) );
		
		for( TVectorVideoConverters::iterator
			it = m_converters.begin(),
			it_end = m_converters.end();
		it != it_end;
		++it )
		{
			const ConstString & name = (*it)->getName();
			CONVERTER_SERVICE(m_serviceProvider)->registerConverter( name, (*it) );
		}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoConverterPlugin::destroy()
	{
		for( TVectorVideoConverters::iterator
			it = m_converters.begin(),
			it_end = m_converters.end();
		it != it_end;
		++it )
		{
			const ConstString & name = (*it)->getName();
			CONVERTER_SERVICE(m_serviceProvider)->unregisterConverter( name );

			(*it)->destroy();
		}

		delete this;
	}
}