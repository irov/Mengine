#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
	class Soundable
        : public Mixin
	{
	public:
		void setVolume( float _volume );
		float getVolume() const;

	protected:
		virtual void _setVolume( float _volume );
		virtual float _getVolume() const;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Soundable> SoundablePtr;
    //////////////////////////////////////////////////////////////////////////
}