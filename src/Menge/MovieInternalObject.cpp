#	include "MovieInternalObject.h"

#	include "Movie.h"

#   include "Logger/Logger.h"

#	include <pybind/pybind.hpp>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MovieInternalObject::MovieInternalObject()
		: m_movie(nullptr)
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
	Node * MovieInternalObject::getInternalNode() const
	{ 
		return m_internalNode;
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

		pybind::object py_object;
		EVENTABLE_ASK( m_serviceProvider, m_movie, EVENT_MOVIE_GET_INTERNAL, py_object )(internalGroup, internalName);

		if( py_object.is_invalid() == true )
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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieInternalObject::_release()
    {
        m_resourceInternalObject.release();

		m_internalObject.reset();				
    }
	//////////////////////////////////////////////////////////////////////////
	bool MovieInternalObject::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

		Node * node = nullptr;
		EVENTABLE_ASK( m_serviceProvider, m_movie, EVENT_MOVIE_ACTIVATE_INTERNAL, node )(m_internalObject);

		if( node == nullptr )
		{
			const ConstString & internalGroup = m_resourceInternalObject->getInternalGroup();
			const ConstString & internalName = m_resourceInternalObject->getInternalName();

			LOGGER_ERROR(m_serviceProvider)("MovieInternalObject::_activate '%s' resource '%s' invalid get internal node '%s:%s'"
				, m_name.c_str()
				, m_resourceInternalObject->getName().c_str()
				, internalGroup.c_str()
				, internalName.c_str()
				);

			return false;
		}

		this->addChild( node );

		m_internalNode = node;

		bool localHide = this->isLocalHide();
		this->updateHide_( localHide );

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
				
		EVENTABLE_CALL(m_serviceProvider, m_movie, EVENT_MOVIE_DEACTIVATE_INTERNAL)( m_internalObject );
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieInternalObject::_localHide( bool _hide )
	{
		if( this->isActivate() == false )
		{
			return;
		}

		this->updateHide_( _hide );
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieInternalObject::_setPersonalColor( const ColourValue& _color )
	{
		this->setLocalColor( _color );
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieInternalObject::_setPersonalAlpha( float _alpha )
	{
		this->setLocalColorAlpha( _alpha );
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieInternalObject::updateHide_( bool _hide )
	{
		if( m_internalNode == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("MovieInternalObject::updateHide_ %s internal Node is nullptr"
				, this->getName().c_str()
				);

			return;
		}

		bool hide = this->isHide();

		m_internalNode->hide( hide | _hide );
	}
}