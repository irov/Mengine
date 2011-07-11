#	include "XmlCodecPlugin.h"

#	include "XmlToBinDecoder.h"

#	include "Utils/Core/File.h"

//////////////////////////////////////////////////////////////////////////
bool initPluginMengeXmlCodec( Menge::PluginInterface ** _plugin )
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
		protected:
			DecoderInterface * createDecoder( InputStreamInterface * _stream ) override
			{
				return new Xml2BinDecoder(_stream);
			}

			void setService( CodecServiceInterface * _service ) override
			{
				//Empty
			}
		};
	}
	//////////////////////////////////////////////////////////////////////////
	XmlCodecPlugin::XmlCodecPlugin()
		: m_xml2bin(0)
		, m_codecs(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlCodecPlugin::initialize( ServiceProviderInterface * _provider )
	{
		ServiceInterface * service = _provider->getService( "Codec" );

		if( service == 0 )
		{
			return;
		}

		m_codecs = static_cast<CodecServiceInterface*>(service);
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlCodecPlugin::run( const TMapParam & _params )
	{
		m_xml2bin = new Detail::Xml2BinSystem();

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