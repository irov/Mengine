#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/CodecInterface.h"
#	include "Interface/StringizeInterface.h"

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
}