#include "Factorable.h"

#include "Interface/FactoryInterface.h"

#include "Kernel/ExceptionHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Factorable::Factorable()
        : m_factorableFactory( nullptr )
        , m_factorableId( INVALID_UNIQUE_ID )
#if defined(MENGINE_FACTORABLE_DEBUG_ENABLE)
        , m_factorableDestroy( false )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Factorable::~Factorable()
    {
#if defined(MENGINE_FACTORABLE_DEBUG_ENABLE)
        if( m_factorableDestroy == false && m_factorableFactory != nullptr )
        {
            MENGINE_THROW_EXCEPTION( "Factorable deleter but not destroy!!" );

            return;
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void Factorable::setFactory( FactoryInterface * _factory )
    {
        m_factorableFactory = _factory;
    }
    //////////////////////////////////////////////////////////////////////////
    void Factorable::setUniqueIdentity( UniqueId _id )
    {
        m_factorableId = _id;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & Factorable::getType() const
    {
        if( m_factorableFactory == nullptr )
        {
            return ConstString::none();
        }

        const ConstString & type = m_factorableFactory->getType();

        return type;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Factorable::incref()
    {
        m_factorableReference.increfReferenceCount();

        uint32_t referenceCount = m_factorableReference.getReferenceCount();

        return referenceCount;
    }
    //////////////////////////////////////////////////////////////////////////
    void Factorable::decref()
    {
        uint32_t referenceCount = m_factorableReference.decrefReferenceCount();

        if( referenceCount != 0 )
        {
            return;
        }

        this->destroy();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Factorable::getrefcount() const
    {
        uint32_t referenceCount = m_factorableReference.getReferenceCount();

        return referenceCount;
    }
    //////////////////////////////////////////////////////////////////////////
    void Factorable::destroy()
    {
#if defined(MENGINE_FACTORABLE_DEBUG_ENABLE)
        if( m_factorableDestroy == true )
        {
            MENGINE_THROW_EXCEPTION( "m_destroy == true" );

            return;
        }

        m_factorableDestroy = true;
#endif

        this->_destroy();

#if defined(MENGINE_FACTORABLE_DEBUG_ENABLE)
        if( m_factorableFactory == nullptr )
        {
            MENGINE_THROW_EXCEPTION( "m_factory == nullptr" );

            return;
        }
#endif

        m_factorableFactory->destroyObject( this );
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_FACTORABLE_DEBUG_ENABLE)
    //////////////////////////////////////////////////////////////////////////
    bool Factorable::isDestroyed() const
    {
        return m_factorableDestroy;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    void Factorable::_destroy()
    {
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////

}
