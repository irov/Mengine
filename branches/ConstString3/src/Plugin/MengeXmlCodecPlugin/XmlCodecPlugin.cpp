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
void releasePluginMengeXmlCodec( Menge::PluginInterface *_plugin )
{
	delete static_cast<Menge::XmlCodecPlugin*>(_plugin);
}
//////////////////////////////////////////////////////////////////////////

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
			DecoderInterface * createDecoder( InputStreamInterface * _stream ) override
			{
				return new Xml2BinDecoder(_stream);
			}
		};
	}
	void XmlCodecPlugin::initialize( ServiceProviderInterface * _provider )
	{
		ServiceInterface * service = _provider->getService( "Codec" );

		if( service == 0 )
		{
			return;
		}

		CodecServiceInterface * codecService = static_cast<CodecServiceInterface*>(service);

		codecService->registerDecoder( "xml2Bin", new Detail::Xml2BinSystem() );
	}
}