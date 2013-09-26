#	pragma once

#   include "Interface/RenderSystemInterface.h"
#   include "Interface/ParticleSystemInterface.h"

#	include "Kernel/ResourceReference.h"

#	include <map>

namespace Menge
{
	class ResourceEmitter
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceEmitter )

	public:
		ResourceEmitter();

	public:
		void setContainer( const ConstString & _container );
		const ConstString & getContainer() const;

		void setEmitterName( const ConstString & _emitterName );
		const ConstString & getEmitterName() const;

		void setEmitterRelative( bool _value );
		bool getEmitterRelative() const;

		void setOffset( const mt::vec2f & _offset );
		const mt::vec2f & getOffset() const;
		
	public:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	private:
		ConstString m_container;
		ConstString m_emitterName;

		bool m_relative;
		mt::vec2f m_offset;
	};
}
