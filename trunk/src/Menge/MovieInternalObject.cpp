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

        EVENTABLE_ASK(m_serviceProvider, m_movie, EVENT_MOVIE_GET_INTERNAL)( py_object, (PyObject*)0, "(ss)", internalGroup.c_str(), internalName.c_str() );

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

		PyObject * py_obj = this->getEmbed();

		bool localHide = this->isLocalHide();
		EVENTABLE_CALL(m_serviceProvider, m_movie, EVENT_MOVIE_HIDE_INTERNAL)( "(OOO)", py_obj, m_internalObject, pybind::get_bool(localHide) );

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

		if( m_internalObject != nullptr )
		{
			PyObject * py_obj = this->getEmbed();
		
			EVENTABLE_CALL(m_serviceProvider, m_movie, EVENT_MOVIE_ACTIVATE_INTERNAL)( "(OO)", py_obj, m_internalObject );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieInternalObject::_deactivate()
	{
		Node::_deactivate();
				
		if( m_internalObject != nullptr )
		{
			PyObject * py_obj = this->getEmbed();

			EVENTABLE_CALL(m_serviceProvider, m_movie, EVENT_MOVIE_DEACTIVATE_INTERNAL)( "(OO)", py_obj, m_internalObject );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieInternalObject::_destroy()
	{		
		pybind::decref( m_internalObject );
		m_internalObject = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieInternalObject::_localHide( bool _hide )
	{
		if( this->isCompile() == false )
		{
			return;
		}

		if( m_internalObject != nullptr )
		{
			PyObject * py_obj = this->getEmbed();

			EVENTABLE_CALL(m_serviceProvider, m_movie, EVENT_MOVIE_HIDE_INTERNAL)( "(OOO)", py_obj, m_internalObject, pybind::get_bool(_hide) );
		}
	}
}