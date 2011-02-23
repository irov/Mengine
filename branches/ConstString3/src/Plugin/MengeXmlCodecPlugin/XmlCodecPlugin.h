#	pragma once

#	include "Interface/PluginInterface.h"

namespace Menge
{
	class DecoderSystemInterface;

	class XmlCodecPlugin
		: public PluginInterface
	{
	public:
		XmlCodecPlugin();

	public:
		void initialize( ServiceProviderInterface * _provider ) override;
		void finalize( ServiceProviderInterface * _provider ) override;

	protected:
		DecoderSystemInterface * m_xml2bin;
	};
}