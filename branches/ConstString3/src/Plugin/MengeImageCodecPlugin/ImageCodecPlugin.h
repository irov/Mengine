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
		CodecDecoderSystem( const ConstString & _name );

	public:
		const ConstString & getName() const;

	protected:
		void setService( CodecServiceInterface * _service ) override;

	protected:
		ConstString m_name;
		CodecServiceInterface * m_service;
	};

	class CodecEncoderSystem
		: public EncoderSystemInterface
	{
	public:
		CodecEncoderSystem( const ConstString & _name );

	public:
		const ConstString & getName() const;

	protected:
		void setService( CodecServiceInterface * _service ) override;

	protected:
		ConstString m_name;
		CodecServiceInterface * m_service;
	};
	
	class ImageCodecPlugin
		: public PluginInterface
	{
	protected:
		void initialize( ServiceProviderInterface * _provider, const TMapParam & _params ) override;
		void finalize() override;

	protected:
		CodecServiceInterface * m_codecService;

		typedef std::vector<CodecDecoderSystem *> TVectorDecoders;
		TVectorDecoders m_decoders;

		typedef std::vector<CodecEncoderSystem *> TVectorEncoders;
		TVectorEncoders m_encoders;
	};
}