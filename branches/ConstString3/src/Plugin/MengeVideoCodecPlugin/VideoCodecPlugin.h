#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/CodecInterface.h"
#	include "Interface/StringizeInterface.h"

namespace Menge
{
	class VideoCodecDecoderSystem;

	class VideoCodecPlugin
		: public PluginInterface
	{
	public:
		VideoCodecPlugin();

	protected:
		void initialize( ServiceProviderInterface * _provider ) override;
		void finalize() override;

	protected:
		typedef std::vector<VideoCodecDecoderSystem *> TVectorVideoDecoders;
		TVectorVideoDecoders m_decoders;

		CodecServiceInterface * m_codecService;
		StringizeServiceInterface * m_stringize;
	};
}

