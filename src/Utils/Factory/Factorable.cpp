#	include "Factorable.h"
#	include "Factory.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    Factorable::Factorable()
	    : m_factory(nullptr)
#   ifdef _DEBUG
        , m_destroy(false)
#   endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Factorable::~Factorable()
    {
#   ifdef _DEBUG
        if( m_destroy == false && m_factory != nullptr )
        {            
            throw FactorableException(); //Factorable delete!!!!!
        }
#   endif
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
            throw FactorableException();
        }
#   endif

#   ifdef _DEBUG
		m_destroy = true;
#   endif

        this->_destroy();

#   ifdef _DEBUG
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
    void Factorable::_destroy()
    {
    }
}
