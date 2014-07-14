#	pragma once

#	include "Kernel/Node.h"
#	include "Kernel/Shape.h"

#   include "ResourceImage.h"

#	include "Core/ColourValue.h"
#	include "Core/ValueInterpolator.h"

#	include "../Interface/RenderSystemInterface.h"

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
        void setResourceImage( ResourceImage * _resourceImage );
        ResourceImage * getResourceImage() const;
        
    public:
        void setCustomSize( const mt::vec2f & _size );

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
        void updateResource_();		
        
	protected:
		bool compileResource_();

	protected:
		ResourceHolder<ResourceImage> m_resourceImage;

        mt::vec2f m_customSize;

		RenderMaterialInterfacePtr m_material;
	
		size_t m_texturesNum;
		RenderTextureInterfacePtr m_textures[2];

		bool m_isCustomSize;
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
