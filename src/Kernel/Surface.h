#pragma once

#include "Kernel/Factorable.h"

#include "Kernel/Identity.h"
#include "Kernel/Visitable.h"
#include "Kernel/Scriptable.h"
#include "Kernel/Materialable.h"
#include "Kernel/Compilable.h"
#include "Kernel/Animatable.h"
#include "Kernel/Eventable.h"
#include "Kernel/Unknowable.h"
#include "Kernel/UpdateContext.h"

#include "Kernel/Color.h"

#include "math/vec2.h"
#include "math/uv4.h"

namespace Mengine
{
    class Surface
        : public Factorable
        , public Identity
        , public Visitable
        , public Scriptable
        , public Materialable
        , public Compilable
        , public Animatable
        , public Eventable
        , public Unknowable
    {
        DECLARE_VISITABLE_BASE();

    public:
        Surface();
        ~Surface() override;

    public:
        void setAnchor( const mt::vec2f & _anchor );
        const mt::vec2f & getAnchor() const;

    public:
        virtual const mt::vec2f & getMaxSize() const = 0;
        virtual const mt::vec2f & getSize() const = 0;
        virtual const mt::vec2f & getOffset() const = 0;

        virtual uint32_t getUVCount() const = 0;
        virtual const mt::uv4f & getUV( uint32_t _index ) const = 0;

        virtual void correctUV( uint32_t _index, const mt::vec2f & _in, mt::vec2f * const _out ) const = 0;

        virtual const Color & getColor() const = 0;

    public:
        bool initialize();
        void finalize();

    protected:
        virtual bool _initialize();
        virtual void _finalize();

    public:
        void activate();
        void deactivate();

    public:
        uint32_t update( const UpdateContext * _context );

    protected:
        virtual void _update( const UpdateContext * _context );

    protected:
        virtual void _activate();
        virtual void _deactivate();

    protected:
        void _invalidateMaterial() const override;

    protected:
        mutable uint32_t m_revision;

        mt::vec2f m_anchor;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Surface> SurfacePtr;
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    using IntrusiveSurfacePtr = IntrusivePtr<T, Surface>;
    //////////////////////////////////////////////////////////////////////////
}
