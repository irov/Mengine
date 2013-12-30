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
	typedef std::vector<ResourceImage *> TVResourceImage;

	struct RenderMaterial;
	struct RenderMaterialGroup;    
	
	class Landscape2D 
		: public Node
	{
	public:
		Landscape2D();
		~Landscape2D();

	public:
		void setBackParts( const TVResourceImage &_resourceImage, int countX, int countY);

	protected:
		bool _compile() override;
		void _release() override;

		void _render( RenderCameraInterface * _camera ) override;

    protected:
		void _invalidateWorldMatrix() override;
		void invalidateVerticesWM();
		void updateVerticesWM();

        void invalidateMaterial();
        void updateMaterial();
        inline const RenderMaterial * getMaterial();
		inline const RenderVertex2D * getVerticesWM();
	
    protected:
        void updateResource_();
		       
	protected:
		bool compileResources_();

	protected:
		std::vector<ResourceImage *>  m_resourceImage; 

		const RenderMaterialGroup * m_materialGroup;
		const RenderMaterial * m_material;

        bool m_invalidateMaterial;

		size_t m_texturesNum;

		std::vector<RenderTextureInterfacePtr> m_textures;

		std::vector<RenderVertex2D> m_verticesWM;

		int m_countX;
		int m_countY;

	private:
		bool m_invalidateVerticesWM;
    };

    //////////////////////////////////////////////////////////////////////////
    inline const RenderMaterial * Landscape2D::getMaterial()
    {
        if( m_invalidateMaterial == true )
        {
            this->updateMaterial();
        }

        return m_material;
    }

	//////////////////////////////////////////////////////////////////////////
	inline const RenderVertex2D * Landscape2D::getVerticesWM()
	{
		if( m_invalidateVerticesWM == true )
		{
			this->updateVerticesWM();
			this->updateVerticesWM();
		}

		mt::vec3f pos(200.f,100.f,0.f);
		this->setLocalPosition( pos ); // !!!!

		return &m_verticesWM[0];
	}
}
