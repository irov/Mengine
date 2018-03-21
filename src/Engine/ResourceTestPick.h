#pragma once

#include "Kernel/ResourceReference.h"

#include "math/vec2.h"

namespace Mengine
{
	class ResourceTestPick
		: public ResourceReference
	{
		DECLARE_VISITABLE( ResourceReference );

	public:
		virtual bool testPoint( const mt::vec2f& _point, float _minAlpha ) const = 0;
		virtual bool testRadius( const mt::vec2f& _point, float _radius, float _minAlpha ) const = 0;

    public:
        virtual uint32_t getWidth() const = 0;
        virtual uint32_t getHeight() const = 0;

    public:
		virtual uint8_t * getBuffer() const = 0;
	};
}	
