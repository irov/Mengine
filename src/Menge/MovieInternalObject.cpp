#	include "MovieInternalObject.h"

#	include "Movie.h"

#   include "Logger/Logger.h"

#	include <pybind/pybind.hpp>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MovieInternalObject::MovieInternalObject()
		: m_movie(nullptr)
		, m_internalObject(nullptr)
		, m_internalNode(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MovieInternalObject::~MovieInternalObject()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieInternalObject::setMovie( Movie * _movie )
	{
		m_movie = _movie;
	}
	//////////////////////////////////////////////////////////////////////////
	Movie * MovieInternalObject::getMovie() const
	{
		return m_movie;
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieInternalObject::setResourceInternalObject( ResourceInternalObject * _resourceInternalObject )
	{
        m_resourceInternalObject = _resourceInternalObject;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceInternalObject * MovieInternalObject::getResourceInternalObject() const
	{
		return m_resourceInternalObject;
	}
    //////////////////////////////////////////////////////////////////////////
    bool MovieInternalObject::_compile()
    {
		if( m_movie == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("MovieInternalObject::_compile '%s' movie not setup"
				, m_name.c_str()
				);

			return false;
		}

        if( m_resourceInternalObject == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("MovieInternalObject::_compile '%s' resource not setup"
                , m_name.c_str()
                );
            
            return false;
        }

        if( m_resourceInternalObject.compile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MovieInternalObject::_compile '%s' resource '%s' not compile"
                , m_name.c_str()
                , m_resourceInternalObject->getName().c_str()
                );

            return false;
        }

        const ConstString & internalGroup = m_resourceInternalObject->getInternalGroup();
        const ConstString & internalName = m_resourceInternalObject->getInternalName();		

        PyObject * py_object = nullptr;

		PyObject * py_internalGroup = pybind::ptr( internalGroup );
		PyObject * py_internalName = pybind::ptr( internalName );

        EVENTABLE_ASK(m_serviceProvider, m_movie, EVENT_MOVIE_GET_INTERNAL)( py_object, (PyObject*)nullptr, "(OO)", py_internalGroup, py_internalName );

		pybind::decref( py_internalGroup );
		pybind::decref( py_internalName );

        if( py_object == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("MovieInternalObject::_compile '%s' resource '%s' can't find internal object '%s:%s'"
                , m_name.c_str()
                , m_resourceInternalObject->getName().c_str()
                , internalGroup.c_str()
                , internalName.c_str()				
                );

            return false;
        }

        m_internalObject = py_object;
        pybind::incref( m_internalObject );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieInternalObject::_release()
    {
        m_resourceInternalObject.release();

		pybind::decref( m_internalObject );
		m_internalObject = nullptr;				
    }
	//////////////////////////////////////////////////////////////////////////
	bool MovieInternalObject::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

		PyObject * py_node = nullptr;
		EVENTABLE_ASK(m_serviceProvider, m_movie, EVENT_MOVIE_ACTIVATE_INTERNAL)( py_node, (PyObject*)nullptr, "(O)", m_internalObject );

		if( py_node == nullptr )
		{
			const ConstString & internalGroup = m_resourceInternalObject->getInternalGroup();
			const ConstString & internalName = m_resourceInternalObject->getInternalName();		

			LOGGER_ERROR(m_serviceProvider)("MovieInternalObject::_activate '%s' resource '%s' can't get internal node '%s:%s'"
				, m_name.c_str()
				, m_resourceInternalObject->getName().c_str()
				, internalGroup.c_str()
				, internalName.c_str()				
				);

			return false;
		}

		Node * node; 
		if( pybind::extract_value( py_node, node ) == false )
		{
			const ConstString & internalGroup = m_resourceInternalObject->getInternalGroup();
			const ConstString & internalName = m_resourceInternalObject->getInternalName();		

			LOGGER_ERROR(m_serviceProvider)("MovieInternalObject::_activate '%s' resource '%s' get internal node '%s:%s' invalid type %s"
				, m_name.c_str()
				, m_resourceInternalObject->getName().c_str()
				, internalGroup.c_str()
				, internalName.c_str()
				, pybind::object_str( py_node )
				);

			return false;
		}

		if( this->addChild( node ) == false )
		{
			const ConstString & internalGroup = m_resourceInternalObject->getInternalGroup();
			const ConstString & internalName = m_resourceInternalObject->getInternalName();		

			LOGGER_ERROR(m_serviceProvider)("MovieInternalObject::_activate '%s' resource '%s' get internal node '%s:%s' invalid add children"
				, m_name.c_str()
				, m_resourceInternalObject->getName().c_str()
				, internalGroup.c_str()
				, internalName.c_str()
				);

			return false;
		}

		m_internalNode = node;

		bool localHide = this->isLocalHide();
		this->updateHide_( localHide );
		//bool localHide = this->isLocalHide();
		//EVENTABLE_CALL(m_serviceProvider, m_movie, EVENT_MOVIE_HIDE_INTERNAL)( "(OO)", m_internalObject, pybind::get_bool(localHide) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieInternalObject::_deactivate()
	{
		Node::_deactivate();

		bool localHide = this->isLocalHide();
		this->updateHide_( localHide );

		m_internalNode->removeFromParent();
		m_internalNode = nullptr;
				
		EVENTABLE_CALL(m_serviceProvider, m_movie, EVENT_MOVIE_DEACTIVATE_INTERNAL)( "(O)", m_internalObject );
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieInternalObject::_localHide( bool _hide )
	{
		if( this->isActivate() == false )
		{
			return;
		}

		this->updateHide_( _hide );
		//EVENTABLE_CALL(m_serviceProvider, m_movie, EVENT_MOVIE_HIDE_INTERNAL)( "(OO)", m_internalObject, pybind::get_bool(_hide) );
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieInternalObject::updateHide_( bool _hide )
	{
		bool hide = this->isHide();
		m_internalNode->hide( hide | _hide );
	}
}