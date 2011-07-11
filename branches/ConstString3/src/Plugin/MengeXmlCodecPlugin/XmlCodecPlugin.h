#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/CodecInterface.h"

namespace Menge
{
	class DecoderSystemInterface;

	class XmlCodecPlugin
		: public PluginInterface
	{
	public:
		XmlCodecPlugin();

	protected:
		void initialize( ServiceProviderInterface * _provider ) override;
		void finalize() override;

	protected:
		void run( const TMapParam & _params ) override;

	protected:
		DecoderSystemInterface * m_xml2bin;

		CodecServiceInterface * m_codecs;
	};
}