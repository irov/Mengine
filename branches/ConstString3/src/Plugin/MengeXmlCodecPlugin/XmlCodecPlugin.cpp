#	include "XmlCodecPlugin.h"

#	include "XmlToBinDecoder.h"

#	include "Interface/StringizeInterface.h"

#	include "Utils/Core/File.h"
#	include "Utils/Core/ConstString.h"

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
			Xml2BinSystem( LogServiceInterface * _logService, UnicodeServiceInterface * _unicodeService )
				: m_logService(_logService)
                , m_unicodeService(_unicodeService)
			{
			}

		protected:
			DecoderInterface * createDecoder( InputStreamInterface * _stream ) override
			{
				return new Xml2BinDecoder(_stream, m_logService, m_unicodeService);
			}

			void setService( CodecServiceInterface * _service ) override
			{
				//Empty
			}

		protected:
			LogServiceInterface * m_logService;
            UnicodeServiceInterface * m_unicodeService;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	XmlCodecPlugin::XmlCodecPlugin()
		: m_provider(0)
		, m_xml2bin(0)
		, m_codecs(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool XmlCodecPlugin::initialize( ServiceProviderInterface * _provider )
	{
		m_provider = _provider;

		CodecServiceInterface * codecService = m_provider->getServiceT<CodecServiceInterface>( "CodecService" );

		if( codecService == 0 )
		{
			return false;
		}

		LogServiceInterface * logService = m_provider->getServiceT<LogServiceInterface>( "LogService" );

		if( logService == 0 )
		{
			return false;
		}

        UnicodeServiceInterface * unicodeService = m_provider->getServiceT<UnicodeServiceInterface>( "UnicodeService" );

        if( unicodeService == 0 )
        {
            return false;
        }
		
		m_xml2bin = new Detail::Xml2BinSystem(logService, unicodeService);

		m_codecs = codecService;

		StringizeServiceInterface * stringizeService = m_provider->getServiceT<StringizeServiceInterface>( "StringizeService" );

		const ConstString & c_xml2bin = stringizeService->stringize("xml2bin");

		m_codecs->registerDecoder( c_xml2bin, m_xml2bin );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlCodecPlugin::finalize()
	{
		StringizeServiceInterface * stringizeService = m_provider->getServiceT<StringizeServiceInterface>( "StringizeService" );

		const ConstString & c_xml2bin = stringizeService->stringize("xml2bin");
				
		m_codecs->unregisterDecoder( c_xml2bin );

		delete static_cast<Detail::Xml2BinSystem *>(m_xml2bin);

		delete this;
	}
}