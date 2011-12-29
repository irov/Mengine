#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/CodecInterface.h"

namespace Menge
{
	
	class SoundCodecDecoderSystem
		: public DecoderSystemInterface
	{
	public:
		SoundCodecDecoderSystem( const String & _name );

	public:
		const String & getName() const;

	protected:
		void setService( CodecServiceInterface * _service ) override;

	protected:
		String m_name;
		CodecServiceInterface * m_service;
	};


	class SoundCodecPlugin
		: public PluginInterface
	{
	public:
		SoundCodecPlugin();

	protected:
		void initialize( ServiceProviderInterface * _provider, const TMapParam & _params ) override;
		void finalize() override;

	protected:
		typedef std::vector<SoundCodecDecoderSystem * > TVectorSoundDecoders;
		TVectorSoundDecoders m_decoders;
		CodecServiceInterface * m_codecService;
		
	};
}