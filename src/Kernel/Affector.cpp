#	include "Kernel/Affector.h"

#	include "Interface/EventInterface.h"

#   include "Kernel/EventEnum.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Affector::Affector()
		: m_serviceProvider(nullptr)
        , m_cb(nullptr)
		, m_type(ETA_POSITION)	
        , m_id(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Affector::~Affector()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void Affector::initialize( ServiceProviderInterface * _serviceProvider, AffectorCallback * _cb, EAffectorType _type )
    {
        m_serviceProvider = _serviceProvider;
        m_cb = _cb;
        m_type = _type;
    }
	//////////////////////////////////////////////////////////////////////////
	void Affector::setId( size_t _id )
	{
		m_id = _id;
	}
    //////////////////////////////////////////////////////////////////////////
    size_t Affector::getId() const
    {
        return m_id;
    }
	//////////////////////////////////////////////////////////////////////////
	EAffectorType Affector::getType() const
	{
		return m_type;
	}
	//////////////////////////////////////////////////////////////////////////
	AffectorCallback * Affector::getCb() const
	{
		return m_cb;
	}
	//////////////////////////////////////////////////////////////////////////
	void Affector::end_( bool _isEnd )
	{
		m_cb->onAffectorEnd( m_id, _isEnd );
	}
}	// namespace Menge
