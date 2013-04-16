#	include "SoundConverterPlugin.h"

#	include "SoundConverterFFMPEGToOGG.h"

#	include "Core/File.h"
#   include "Codec/ConverterFactory.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/StringizeInterface.h"

//////////////////////////////////////////////////////////////////////////
__declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
{
	*_plugin = new Menge::SoundConverterPlugin();
	return true;
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SoundConverterPlugin::SoundConverterPlugin()
        : m_serviceProvider(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundConverterPlugin::initialize( ServiceProviderInterface * _serviceProvider )
	{
        m_serviceProvider = _serviceProvider;

		ConstString c_ffmpegToOgg = Helper::stringizeString(m_serviceProvider, "ffmpegToOggSound");

		m_converters.push_back( new ConverterFactory<SoundConverterFFMPEGToOGG>(m_serviceProvider, c_ffmpegToOgg) );
		
		for( TVectorSoundConverters::iterator
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
	void SoundConverterPlugin::destroy()
	{
		for( TVectorSoundConverters::iterator
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