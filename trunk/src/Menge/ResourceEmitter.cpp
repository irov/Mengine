#	include "ResourceEmitter.h"
#	include "Kernel/ResourceImplement.h"

#   include "Interface/ResourceInterface.h"
#   include "Interface/ParticleSystemInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Logger/Logger.h"

#	include "Consts.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceEmitter );
	//////////////////////////////////////////////////////////////////////////
	ResourceEmitter::ResourceEmitter()
		: m_relative(false)
		, m_offset(0.f, 0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitter::setContainer( const ConstString & _container )
	{
		m_container = _container;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceEmitter::getContainer() const
	{
		return m_container;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitter::setEmitterName( const ConstString & _emitterName )
	{
		m_emitterName = _emitterName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceEmitter::getEmitterName() const
	{
		return m_emitterName;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitter::setEmitterRelative( bool _value )
	{
		m_relative = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceEmitter::getEmitterRelative() const
	{
		return m_relative;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitter::setOffset( const mt::vec2f & _offset )
	{
		m_offset = _offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceEmitter::getOffset() const
	{
		return m_offset;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceEmitter::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceEmitter * metadata
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceEmitter *>(_meta);

        m_container = metadata->get_Container_Name();
        m_emitterName = metadata->get_Emitter_Name();

		m_relative = metadata->get_EmitterRelative_Value();

		metadata->get_Offset_Value( m_offset );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
}
