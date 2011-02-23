#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/CodecInterface.h"

#	include <vector>

namespace Menge
{
	class CodecDecoderSystem
		: public DecoderSystemInterface
	{
	public:
		CodecDecoderSystem( const String & _name );

	public:
		const String & getName() const;

	protected:
		void setService( CodecServiceInterface * _service ) override;

	protected:
		String m_name;
		CodecServiceInterface * m_service;
	};

	class CodecEncoderSystem
		: public EncoderSystemInterface
	{
	public:
		CodecEncoderSystem( const String & _name );

	public:
		const String & getName() const;

	protected:
		void setService( CodecServiceInterface * _service ) override;

	protected:
		String m_name;
		CodecServiceInterface * m_service;
	};
	
	class ImageCodecPlugin
		: public PluginInterface
	{
	public:
		void initialize( ServiceProviderInterface * _provider ) override;
		void finalize( ServiceProviderInterface * _provider ) override;

	protected:
		typedef std::vector<CodecDecoderSystem *> TVectorDecoders;
		TVectorDecoders m_decoders;

		typedef std::vector<CodecEncoderSystem *> TVectorEncoders;
		TVectorEncoders m_encoders;
	};
}