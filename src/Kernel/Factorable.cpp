#include "Factorable.h"
#include "Factory.h"

#include "Kernel/Exception.h"
#include "Kernel/ThreadGuardScope.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Factorable::Factorable()
        : m_reference( 0 )
        , m_factory( nullptr )
#if MENGINE_FACTORABLE_DEBUG
        , m_destroy( false )
        , m_immortal( false )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Factorable::~Factorable()
    {
#if MENGINE_FACTORABLE_DEBUG
        if( m_destroy == false && m_factory != nullptr )
        {
            MENGINE_THROW_EXCEPTION( "Factorable deleter but not destroy!!" );
        }

        if( m_reference != 0 )
        {
            MENGINE_THROW_EXCEPTION( "m_reference %u != 0"
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
    const ConstString & Factorable::getType() const
    {
        if( m_factory == nullptr )
        {
            return ConstString::none();
        }

        const ConstString & type = m_factory->getType();

        return type;
    }
    //////////////////////////////////////////////////////////////////////////
    void Factorable::setImmortal( bool _value )
    {
        MENGINE_UNUSED( _value );

#if MENGINE_FACTORABLE_DEBUG
        m_immortal = _value;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool Factorable::getImmortal() const
    {
#if MENGINE_FACTORABLE_DEBUG
        return m_immortal;
#else
        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Factorable::incref()
    {
        MENGINE_THREAD_GUARD_SCOPE( Factorable, this, "Factorable::incref" );

        ++m_reference;

        return m_reference;
    }
    //////////////////////////////////////////////////////////////////////////
    void Factorable::decref()
    {
        {
            MENGINE_THREAD_GUARD_SCOPE( Factorable, this, "Factorable::decref" );

            --m_reference;
        }

        if( m_reference == 0 )
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
#if MENGINE_FACTORABLE_DEBUG
        if( m_immortal == true )
        {
            MENGINE_THROW_EXCEPTION( "m_immortal == true" );
        }
#endif

#if MENGINE_FACTORABLE_DEBUG
        if( m_destroy == true )
        {
            MENGINE_THROW_EXCEPTION( "m_destroy == true" );
        }

        m_destroy = true;
#endif

        this->_destroy();

#if MENGINE_FACTORABLE_DEBUG
        this->_checkDestroy();
#endif

#if MENGINE_FACTORABLE_DEBUG
        if( m_factory == nullptr )
        {
            MENGINE_THROW_EXCEPTION( "m_factory == nullptr" );
        }
#endif

        m_factory->destroyObject( this );
    }
    //////////////////////////////////////////////////////////////////////////
#if MENGINE_FACTORABLE_DEBUG
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
#if MENGINE_FACTORABLE_DEBUG
    void Factorable::_checkDestroy()
    {
        if( m_reference != 0 )
        {
            MENGINE_THROW_EXCEPTION( "m_reference %u != 0"
                , m_reference
            );
        }
    }
#endif
    //////////////////////////////////////////////////////////////////////////

}
