#pragma once

#include "Interface/UpdateInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Updatable
        : public Mixin
    {
    public:
        Updatable();
        ~Updatable() override;

    public:
        virtual void update( const UpdateContext * _context );

    protected:
        virtual void _update( const UpdateContext * _context ) = 0;

    protected:
        void setUpdateRevision( uint32_t _revision );
        inline uint32_t getUpdateRevision() const;

    protected:
        uint32_t m_updateRevision;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Updatable> UpdatablePtr;
    //////////////////////////////////////////////////////////////////////////
    inline uint32_t Updatable::getUpdateRevision() const
    {
        return m_updateRevision;
    }
    //////////////////////////////////////////////////////////////////////////
}
