#pragma once

#include "Kernel/Factorable.h"

#include "Kernel/Identity.h"
#include "Kernel/Servant.h"
#include "Kernel/Visitable.h"
#include "Kernel/Scriptable.h"
#include "Kernel/Materialable.h"
#include "Kernel/Compilable.h"
#include "Kernel/Animatable.h"
#include "Kernel/Eventable.h"
#include "Kernel/Unknowable.h"

#include "math/vec2.h"
#include "math/uv4.h"
#include "Kernel/ColourValue.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct UpdateContext;
    //////////////////////////////////////////////////////////////////////////
    class Surface
        : public Servant
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
        virtual const mt::vec2f & getMaxSize() const = 0;
        virtual const mt::vec2f & getSize() const = 0;
        virtual const mt::vec2f & getOffset() const = 0;

        virtual uint32_t getUVCount() const = 0;
        virtual const mt::uv4f & getUV( uint32_t _index ) const = 0;

        virtual void correctUV( uint32_t _index, mt::vec2f & _out, const mt::vec2f & _in ) = 0;

        virtual const ColourValue & getColor() const = 0;

    public:
        bool initialize();

    protected:
        virtual bool _initialize();

    public:
        void activate();
        void deactivate();

    public:
        bool update( const UpdateContext * _context );

    protected:
        virtual void _activate();
        virtual void _deactivate();
        virtual bool _update( const UpdateContext * _context );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Surface> SurfacePtr;
}
