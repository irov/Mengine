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
		void initialize( ServiceProviderInterface * _provider, const TMapParam & _params ) override;
		void finalize() override;

	protected:
		ServiceProviderInterface * m_provider;

		DecoderSystemInterface * m_xml2bin;

		CodecServiceInterface * m_codecs;
	};
}