#	pragma once

#	include "Kernel/ResourceReference.h"

#   include "Math/vec2.h"

namespace Menge
{
	class ResourceTestPick
		: public ResourceReference
	{
	public:
		virtual bool testPoint( const mt::vec2f& _point, float _minAlpha ) const = 0;
		virtual bool testRadius( const mt::vec2f& _point, float _radius, float _minAlpha ) const = 0;

    public:
        virtual size_t getWidth() const = 0;
        virtual size_t getHeight() const = 0;

    public:
        virtual unsigned char * getBuffer() const = 0;
	};
}	// namespace Menge
