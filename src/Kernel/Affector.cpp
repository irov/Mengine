#	include "Kernel/Affector.h"

#	include "Interface/EventInterface.h"

#   include "Kernel/EventEnum.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Affector::Affector( ServiceProviderInterface * _serviceProvider, PyObject* _cb, EAffectorType _type )
		: m_serviceProvider(_serviceProvider)
        , m_cb(_cb)
		, m_type(_type)	
	{
		pybind::incref( m_cb );
	}
	//////////////////////////////////////////////////////////////////////////
	Affector::~Affector()
	{
		pybind::decref( m_cb );
	}
	//////////////////////////////////////////////////////////////////////////
	void Affector::setId( size_t _id )
	{
		m_id = _id;
	}
    size_t Affector::getId() const
    {
        return m_id;
    }
	//////////////////////////////////////////////////////////////////////////
	void Affector::setType( EAffectorType _type )
	{
		m_type = _type;
	}
	//////////////////////////////////////////////////////////////////////////
	EAffectorType Affector::getType() const
	{
		return m_type;
	}
	//////////////////////////////////////////////////////////////////////////
	void Affector::setCb( PyObject * _cb )
	{
		m_cb = _cb;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Affector::getCb() const
	{
		return m_cb;
	}
	//////////////////////////////////////////////////////////////////////////
	void Affector::call( Scriptable * _scriptable, bool _isEnd )
	{
		if( m_cb == 0 )
		{
			return;
		}

		if( pybind::is_none(m_cb) == true )
		{
			return;
		}

        SCRIPT_SERVICE(m_serviceProvider)
            ->callFunction(m_cb, "(OiO)", _scriptable->getEmbed(), m_id, pybind::get_bool(_isEnd) );
        //EVENT_SERVICE(m_serviceProvider)
            //->addEventFormat( EVENT_AFFECTOR_END, m_cb, "(OiO)", _scriptable->getEmbed(), m_id, pybind::get_bool(_isEnd) );
        //pybind::call( m_cb, "(OiO)", _scriptable->getEmbed(), m_id, pybind::get_bool(_isEnd) );
	}
}	// namespace Menge
