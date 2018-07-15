#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
	class Updatable
        : public Mixin
	{
	public:
		virtual void update( float _current, float _timing );

	protected:
		virtual void _update( float _current, float _timing ) = 0;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Updatable> UpdatablePtr;
    //////////////////////////////////////////////////////////////////////////
}
