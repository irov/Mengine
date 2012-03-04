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
			Xml2BinSystem( LogServiceInterface * _logService )
				: m_logService(_logService)
			{
			}

		protected:
			DecoderInterface * createDecoder( InputStreamInterface * _stream ) override
			{
				return new Xml2BinDecoder(_stream, m_logService);
			}

			void setService( CodecServiceInterface * _service ) override
			{
				//Empty
			}

		protected:
			LogServiceInterface * m_logService;
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
		CodecServiceInterface * codecService = _provider->getServiceT<CodecServiceInterface>( "CodecService" );

		if( codecService == 0 )
		{
			return;
		}

		LogServiceInterface * logService = _provider->getServiceT<LogServiceInterface>( "LogService" );

		if( logService == 0 )
		{
			return;
		}
		
		m_xml2bin = new Detail::Xml2BinSystem(logService);

		m_codecs = codecService;
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