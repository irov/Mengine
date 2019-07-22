#include "Factorable.h"
#include "Factory.h"

#include "Kernel/Exception.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Factorable::Factorable()
        : m_reference( 0 )
        , m_factory( nullptr )
#ifdef MENGINE_FACTORABLE_DEBUG
        , m_destroy( false )
        , m_immortal( false )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Factorable::~Factorable()
    {
#ifdef MENGINE_FACTORABLE_DEBUG
        if( m_destroy == false && m_factory != nullptr )
        {
            MENGINE_THROW_EXCEPTION( "Factorable deleter but not destroy!!" );
        }

        if( m_reference != 0 )
        {
            MENGINE_THROW_EXCEPTION( "m_reference %d != 0"
                , m_reference
            );
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void Factorable::setFactory( Factory * _factory )
    {
        m_factory = _factory;
    }
    //////////////////////////////////////////////////////////////////////////
    void Factorable::setImmortal( bool _value )
    {
        MENGINE_UNUSED( _value );

#ifdef MENGINE_FACTORABLE_DEBUG
        m_immortal = _value;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Factorable::incref()
    {
        ++m_reference;

        return m_reference;
    }
    //////////////////////////////////////////////////////////////////////////
    void Factorable::decref()
    {
        if( --m_reference == 0 )
        {
            this->destroy();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Factorable::getrefcount() const
    {
        return m_reference;
    }
    //////////////////////////////////////////////////////////////////////////
    void Factorable::destroy()
    {
#ifdef MENGINE_FACTORABLE_DEBUG
        if( m_immortal == true )
        {
            MENGINE_THROW_EXCEPTION( "m_immortal == true" );
        }
#endif

#ifdef MENGINE_FACTORABLE_DEBUG
        if( m_destroy == true )
        {
            MENGINE_THROW_EXCEPTION( "m_destroy == true" );
        }

        m_destroy = true;
#endif

        this->_destroy();

#ifdef MENGINE_FACTORABLE_DEBUG
        this->_checkDestroy();
#endif

#ifdef MENGINE_FACTORABLE_DEBUG
        if( m_factory == nullptr )
        {
            MENGINE_THROW_EXCEPTION( "m_factory == nullptr" );
        }
#endif

        m_factory->destroyObject( this );
    }
    //////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_FACTORABLE_DEBUG
    //////////////////////////////////////////////////////////////////////////
    bool Factorable::isDestroyed() const
    {
        return m_destroy;
    }
#endif
    //////////////////////////////////////////////////////////////////////////
    void Factorable::_destroy()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Factorable::getReference() const
    {
        return m_reference;
    }
    //////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_FACTORABLE_DEBUG
    void Factorable::_checkDestroy()
    {
        if( m_reference != 0 )
        {
            MENGINE_THROW_EXCEPTION( "m_reference %d != 0"
                , m_reference
            );
        }
    }
#endif
}
