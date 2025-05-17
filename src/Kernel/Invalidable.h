#pragma once

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Invalidable
    {
    public:
        Invalidable();
        ~Invalidable();

    protected:
        template<typename T>
        struct FlagGenerator
        {
            static constexpr uint32_t value()
            {
                return 1U << FlagGenerator<T>::FlagIndex;
            }

        private:
            static constexpr uint32_t FlagIndex = __COUNTER__;
        };

    public:
        void validate( uint32_t _flags ) const;
        void invalidate( uint32_t _flags ) const;

    public:
        MENGINE_INLINE bool isInvalid( uint32_t _flags ) const;

    private:
        mutable uint32_t m_invalidateFlags;
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Invalidable::isInvalid( uint32_t _flags ) const
    {
        return (m_invalidateFlags & _flags) != _flags;
    }
    //////////////////////////////////////////////////////////////////////////
}
