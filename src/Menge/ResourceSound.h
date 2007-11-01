#	pragma once

#	include "ResourceReference.h"

class SoundBufferInterface;

namespace Menge
{
	class ResourceSound
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceSound )

	public:
		ResourceSound( const std::string & _name );

	public:
		void loader( TiXmlElement * _xml ) override;

	public:
		SoundBufferInterface * get();

	protected:
		bool _compile() override;
		void _release() override;

	private:
		std::string	m_filename;
		bool m_isStreamable;
		SoundBufferInterface * m_interface;
	};
}