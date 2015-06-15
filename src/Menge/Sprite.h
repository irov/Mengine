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
	struct RenderMaterial;
	struct RenderMaterialGroup;    
	
	class Sprite
		: public Shape
	{
	public:
		Sprite();
		~Sprite();

	public:
		void setMaterialName( const ConstString & _materialName );
		const ConstString & getMaterialName() const;

	protected:
		bool _compile() override;
		void _release() override;

		void _render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera ) override;

		void _updateBoundingBox( mt::box2f & _boundingBox ) const override;
        void _invalidateColor() override;

    protected:
        void updateMaterial();
        inline const RenderMaterialInterfacePtr & getMaterial();

	protected:
		RenderMaterialInterfacePtr m_material;

		ConstString m_materialName;
    };
    //////////////////////////////////////////////////////////////////////////
    inline const RenderMaterialInterfacePtr & Sprite::getMaterial()
    {
        if( this->isInvalidateMaterial() == true )
        {
            this->updateMaterial();
        }

        return m_material;
    }
}
