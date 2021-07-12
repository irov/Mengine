#include "Factorable.h"
#include "Factory.h"

#include "Kernel/Exception.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Factorable::Factorable()
        : m_factory( nullptr )
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
        m_reference.incref();

        uint32_t count = m_reference.getReferenceCount();

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
    void Factorable::decref()
    {
        if( m_reference.decref() == false )
        {
            return;
        }

        this->destroy();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Factorable::getrefcount() const
    {
        uint32_t count = m_reference.getReferenceCount();

        return count;
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

}
