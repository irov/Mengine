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

#include "math/vec2.h"
#include "math/uv4.h"
#include "Kernel/ColourValue.h"

namespace Mengine
{
	class Surface
		: public Servant
        , public Identity
        , public Visitable
        , public Scriptable
        , public Materialable
        , public Compilable
        , public Animatable
        , public Eventable
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
        bool update( float _current, float _timing );

    protected:
        virtual bool _update( float _current, float _timing ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<Surface> SurfacePtr;
}
