#pragma once

#include "Interface/EasingInterface.h"

#include "Kernel/AffectorType.h"
#include "Kernel/Updatable.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/Factorable.h"
#include "Kernel/UpdateContext.h"
#include "Kernel/IntrusiveSlugLinkedPtr.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Affector
        : public IntrusiveSlugLinkedPtr<Affector, void, IntrusivePtr, IntrusivePtrBase>
        , public Factorable
        , public Updatable
        , protected BaseUpdation
    {
        DECLARE_UPDATABLE();

    public:
        Affector();
        ~Affector() override;

    public:
        void setAffectorType( EAffectorType _type ) noexcept;
        EAffectorType getAffectorType() const noexcept;

    public:
        void setId( UniqueId _id ) noexcept;
        UniqueId getId() const noexcept;

    public:
        void setFreeze( bool _value ) noexcept;
        bool getFreeze() const noexcept;

    public:
        void setEasing( const EasingInterfacePtr & _easing );
        const EasingInterfacePtr & getEasing() const;

    protected:
        virtual void _setFreeze( bool _value );

    public:
        void setSpeedFactor( float _speed ) noexcept;
        float getSpeedFactor() const noexcept;

    public:
        bool prepare( EUpdateMode _updatableMode, uint32_t _updatableLeaf );

    protected:
        virtual bool _prepare();

    protected:
        void update( const UpdateContext * _context ) override;

    protected:
        virtual bool _affect( const UpdateContext * _context, float * const _used ) = 0;

    public:
        void stop();

    protected:
        virtual void _stop();

    protected:
        void complete( bool _isEnd );

    protected:
        virtual void _complete( bool _isEnd );

    protected:
        EAffectorType m_type;
        UniqueId m_id;

        EasingInterfacePtr m_easing;

        uint32_t m_updataterId;

        float m_speedFactor;

        bool m_freeze;
        bool m_affecting;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Affector> AffectorPtr;
    //////////////////////////////////////////////////////////////////////////
}
