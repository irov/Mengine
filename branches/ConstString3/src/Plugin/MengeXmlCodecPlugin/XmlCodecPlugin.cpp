#	include "XmlCodecPlugin.h"

#	include "Interface/CodecInterface.h"

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

		CodecServiceInterface * codecService = static_cast<CodecServiceInterface*>(service);

		m_xml2bin = new Detail::Xml2BinSystem();

		codecService->registerDecoder( "xml2bin", m_xml2bin );
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlCodecPlugin::finalize( ServiceProviderInterface * _provider )
	{
		delete static_cast<Detail::Xml2BinSystem *>(m_xml2bin);

		delete this;
	}
}