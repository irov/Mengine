#pragma once

#include "Kernel/AffectorType.h"
#include "Kernel/Updatable.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/Factorable.h"
#include "Kernel/UpdateContext.h"

#include "stdex/intrusive_slug_linked_ptr.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AffectorCallbackInterface
        : public Factorable
    {
    public:
        virtual void onAffectorEnd( AFFECTOR_ID _id, bool _isEnd ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AffectorCallbackInterface> AffectorCallbackInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class Affector
        : public stdex::intrusive_slug_linked_ptr<Affector, void, IntrusivePtr, IntrusivePtrBase>
        , public Factorable
        , public Updatable
        , public BaseUpdation
    {
        DECLARE_UPDATABLE();

    public:
        Affector();
        ~Affector() override;

    public:
        void setAffectorType( EAffectorType _type ) noexcept;
        EAffectorType getAffectorType() const noexcept;

    public:
        void setId( AFFECTOR_ID _id ) noexcept;
        AFFECTOR_ID getId() const noexcept;

    public:
        void setFreeze( bool _value ) noexcept;
        bool getFreeze() const noexcept;

    protected:
        virtual void _setFreeze( bool _value );

    public:
        void setSpeedFactor( float _speed ) noexcept;
        float getSpeedFactor() const noexcept;

    public:
        bool prepare( uint32_t _updatableMode, uint32_t _updatableLeaf );

    protected:
        virtual bool _prepare();

    protected:
        void _update( const UpdateContext * _context ) override;

    protected:
        virtual bool _affect( const UpdateContext * _context, float * _used ) = 0;

    public:
        void stop();

    protected:
        virtual void _stop();

    protected:
        void complete( bool _isEnd );

        virtual void _complete( bool _isEnd );

    protected:
        EAffectorType m_type;
        AFFECTOR_ID m_id;

        uint32_t m_updataterId;

        float m_speedFactor;

        bool m_freeze;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Affector> AffectorPtr;
    //////////////////////////////////////////////////////////////////////////

}
