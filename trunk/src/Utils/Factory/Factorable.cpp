#	include "Factorable.h"
#	include "Factory.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    Factorable::Factorable()
	    : m_factory(0)
#   ifdef _DEBUG
        , m_destroy(false)
#   endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Factorable::~Factorable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Factorable::setFactory( Factory * _factory )
    {
	    m_factory = _factory;
    }
    //////////////////////////////////////////////////////////////////////////
    void Factorable::destroy()
    {
#   ifdef _DEBUG
        if( m_destroy == true )
        {
            throw std::exception();
        }
#   endif

        if( this->_destroy() == false )
        {
            return;
        }

#   ifdef _DEBUG
        m_destroy = true;

        this->_checkDestroy();
#   endif

        m_factory->destroyObject( this );
    }
#   ifdef _DEBUG
    //////////////////////////////////////////////////////////////////////////
    void Factorable::_checkDestroy()
    {
        //Empty
    }
#   endif
    //////////////////////////////////////////////////////////////////////////
    bool Factorable::_destroy()
    {
	    return true;
    }
}
