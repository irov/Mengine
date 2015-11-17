#	pragma once

#	include "Kernel/Node.h"
#	include "Kernel/Shape.h"

#   include "Kernel/ResourceImage.h"

#	include "Core/ColourValue.h"
#	include "Core/ValueInterpolator.h"

#	include "Interface/RenderSystemInterface.h"

#	include "math/mat3.h"
#	include "math/vec4.h"
#	include "math/mat4.h"

namespace Menge
{
	class Sprite
		: public Shape
	{
	public:
		Sprite();
		~Sprite();

	protected:
		bool _compile() override;
		void _release() override;

		void _render( const RenderObjectState * _state ) override;

		void _updateBoundingBox( mt::box2f & _boundingBox ) const override;
        void _invalidateColor() override;

    protected:
		RenderMaterialInterfacePtr _updateMaterial() const override;
    };
}
