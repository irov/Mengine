#	include "ResourceEmitter2.h"

#	include "Kernel/ResourceImplement.h"

#   include "Interface/ResourceInterface.h"
#   include "Interface/ParticleSystemInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Logger/Logger.h"

#	include "Consts.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceEmitter2 );
	//////////////////////////////////////////////////////////////////////////
	ResourceEmitter2::ResourceEmitter2()
		: m_relative(false)
		, m_offset(0.f, 0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitter2::setContainer( const ConstString & _container )
	{
		m_container = _container;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceEmitter2::getContainer() const
	{
		return m_container;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitter2::setEmitterName( const ConstString & _emitterName )
	{
		m_emitterName = _emitterName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceEmitter2::getEmitterName() const
	{
		return m_emitterName;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitter2::setEmitterRelative( bool _value )
	{
		m_relative = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceEmitter2::getEmitterRelative() const
	{
		return m_relative;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitter2::setOffset( const mt::vec2f & _offset )
	{
		m_offset = _offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceEmitter2::getOffset() const
	{
		return m_offset;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceEmitter2::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceEmitter2 * metadata
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceEmitter2 *>(_meta);

        m_container = metadata->get_Container_Name();
        m_emitterName = metadata->get_Emitter_Name();

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
}
