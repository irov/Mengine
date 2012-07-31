#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/CodecInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/LogSystemInterface.h"

namespace Menge
{	
	class VideoCodecDecoderSystem
		: public DecoderSystemInterface
	{
	public:
		VideoCodecDecoderSystem( const ConstString & _name, LogServiceInterface * _logSystem );

	public:
		const ConstString & getName() const;

	public:
		bool initialize(); 

	protected:
		void setService( CodecServiceInterface * _service ) override;

	protected:
		DecoderInterface * createDecoder( InputStreamInterface * _stream ) override;

	protected:
		ConstString m_name;		
		LogServiceInterface * m_logSystem;

		CodecServiceInterface * m_service;
	};
}