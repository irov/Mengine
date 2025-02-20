#include "Factorable.h"

#include "Interface/FactoryInterface.h"

#include "Kernel/ExceptionHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Factorable::Factorable()
        : m_factory( nullptr )
#if defined(MENGINE_FACTORABLE_DEBUG_ENABLE)
        , m_destroy( false )
        , m_immortal( false )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Factorable::~Factorable()
    {
#if defined(MENGINE_FACTORABLE_DEBUG_ENABLE)
        if( m_destroy == false && m_factory != nullptr )
        {
            MENGINE_THROW_EXCEPTION( "Factorable deleter but not destroy!!" );

            return;
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void Factorable::setFactory( FactoryInterface * _factory )
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

#if defined(MENGINE_FACTORABLE_DEBUG_ENABLE)
        m_immortal = _value;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool Factorable::getImmortal() const
    {
#if defined(MENGINE_FACTORABLE_DEBUG_ENABLE)
        return m_immortal;
#else
        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Factorable::incref() noexcept
    {
        m_reference.incref();

        uint32_t count = m_reference.getReferenceCount();

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
    void Factorable::decref() noexcept
    {
        if( m_reference.decref() != 0 )
        {
            return;
        }

        this->destroy();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Factorable::getrefcount() const noexcept
    {
        uint32_t count = m_reference.getReferenceCount();

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
    void Factorable::destroy()
    {
#if defined(MENGINE_FACTORABLE_DEBUG_ENABLE)
        if( m_immortal == true )
        {
            MENGINE_THROW_EXCEPTION( "m_immortal == true" );

            return;
        }
#endif

#if defined(MENGINE_FACTORABLE_DEBUG_ENABLE)
        if( m_destroy == true )
        {
            MENGINE_THROW_EXCEPTION( "m_destroy == true" );

            return;
        }

        m_destroy = true;
#endif

        this->_destroy();

#if defined(MENGINE_FACTORABLE_DEBUG_ENABLE)
        if( m_factory == nullptr )
        {
            MENGINE_THROW_EXCEPTION( "m_factory == nullptr" );

            return;
        }
#endif

        m_factory->destroyObject( this );
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_FACTORABLE_DEBUG_ENABLE)
    //////////////////////////////////////////////////////////////////////////
    bool Factorable::isDestroyed() const
    {
        return m_destroy;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    void Factorable::_destroy()
    {
    }
    //////////////////////////////////////////////////////////////////////////

}
