#	include "XmlCodecPlugin.h"

#	include "XmlToBinDecoder.h"

#	include "Utils/Core/File.h"

//////////////////////////////////////////////////////////////////////////
__declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
{
	*_plugin = new Menge::XmlCodecPlugin();
	return true;
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	namespace Detail
	{
		class Xml2BinSystem
			: public DecoderSystemInterface
		{
		public:
			Xml2BinSystem( LogSystemInterface * _logSystem )
				: m_logSystem(_logSystem)
			{
			}

		protected:
			DecoderInterface * createDecoder( InputStreamInterface * _stream ) override
			{
				return new Xml2BinDecoder(_stream, m_logSystem);
			}

			void setService( CodecServiceInterface * _service ) override
			{
				//Empty
			}

		protected:
			LogSystemInterface * m_logSystem;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	XmlCodecPlugin::XmlCodecPlugin()
		: m_xml2bin(0)
		, m_codecs(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlCodecPlugin::initialize( ServiceProviderInterface * _provider, const TMapParam & _params )
	{
		CodecServiceInterface * codec_service = _provider->getServiceT<CodecServiceInterface>( "Codec" );

		if( codec_service == 0 )
		{
			return;
		}

		LogServiceInterface * logger_service = _provider->getServiceT<LogServiceInterface>( "Log" );

		if( logger_service == 0 )
		{
			return;
		}
		
		LogSystemInterface * logSystem = logger_service->getInterface();

		m_xml2bin = new Detail::Xml2BinSystem(logSystem);

		m_codecs = codec_service;
		m_codecs->registerDecoder( "xml2bin", m_xml2bin );
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlCodecPlugin::finalize()
	{
		m_codecs->unregisterDecoder( "xml2bin" );

		delete static_cast<Detail::Xml2BinSystem *>(m_xml2bin);

		delete this;
	}
}