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
		void disableTextureColor( bool _disable );

		void setBlendAdd( bool _value );
		bool isBlendAdd() const;

        void setCustomSize( const mt::vec2f & _size );

	protected:
		bool _compile() override;
		void _release() override;

		void _render( RenderCameraInterface * _camera ) override;

		void _updateBoundingBox( mt::box2f & _boundingBox ) override;
        void _invalidateColor() override;

    protected:
        void invalidateMaterial();
        void updateMaterial();
        const RenderMaterial * getMaterial();

    protected:
        void updateResource_();
		
        
	protected:
		bool compileResource_();

	protected:
		ResourceHolder<ResourceImage> m_resourceImage;

        bool m_isCustomSize;
        mt::vec2f m_customSize;

		bool m_blendAdd;
		bool m_solid;

		const RenderMaterialGroup * m_materialGroup;
		const RenderMaterial * m_material;

        bool m_invalidateMaterial;

		bool m_disableTextureColor;
		size_t m_texturesNum;
		mt::vec2f m_textureMatrixOffset;

		RenderTextureInterfacePtr m_textures[2];

        VBHandle m_maskVB;
        IBHandle m_maskIB;
    };
    //////////////////////////////////////////////////////////////////////////
    inline const RenderMaterial * Sprite::getMaterial()
    {
        if( m_invalidateMaterial == true )
        {
            this->updateMaterial();
        }

        return m_material;
    }
}
