#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/CodecInterface.h"

namespace Menge
{
	
	class VideoCodecDecoderSystem
		: public DecoderSystemInterface
	{
	public:
		VideoCodecDecoderSystem( const ConstString & _name );

	public:
		const ConstString & getName() const;

	protected:
		void setService( CodecServiceInterface * _service ) override;

	protected:
		ConstString m_name;
		CodecServiceInterface * m_service;
	};


	class VideoCodecPlugin
		: public PluginInterface
	{
	public:
		VideoCodecPlugin();

	protected:
		void initialize( ServiceProviderInterface * _provider, const TMapParam & _params ) override;
		void finalize() override;

	protected:
		typedef std::vector<VideoCodecDecoderSystem * > TVectorVideoDecoders;
		TVectorVideoDecoders m_decoders;
		CodecServiceInterface * m_codecService;
		
	};
}