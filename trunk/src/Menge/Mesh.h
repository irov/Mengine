#	pragma once

#	include "Kernel/Node.h"

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
	
#	define MENGINE_MESH_MAX_VERTEX 32
#	define MENGINE_MESH_MAX_INDECIES ((MENGINE_MESH_MAX_VERTEX - 2) * 3)

	class Mesh
		: public Node
	{
	public:
		Mesh();
		~Mesh();

    public:
        void setResourceImage( ResourceImage * _resourceImage );
        ResourceImage * getResourceImage() const;
        
    public:
		void setBlendAdd( bool _value );
		bool isBlendAdd() const;

	public:
		void setTextureUVOffset( const mt::vec2f & _offset );
		const mt::vec2f & getTextureUVOffset() const;

		void setTextureUVScale( const mt::vec2f & _scale );
		const mt::vec2f & getTextureUVScale() const;

	public:
		void setVerticies( const mt::vec3f * _position, const mt::vec2f * _uv, size_t _countVertex, const uint16 * _indicies, size_t _countIndex );

	protected:
		bool _compile() override;
		void _release() override;

		void _render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera ) override;

		void _updateBoundingBox( mt::box2f & _boundingBox ) override;
        void _invalidateColor() override;
		void _invalidateWorldMatrix() override;

    protected:
        void invalidateMaterial();
        void updateMaterial();
        inline const RenderMaterial * getMaterial();

    protected:
        void updateResource_();
		        
	protected:
		bool compileResource_();

	protected:
		void invalidateVertices();
		void updateVertices();

	protected:
		void invalidateVerticesWM();
		void updateVerticesWM();

	protected:
		void invalidateVerticesColor();
		void updateVerticesColor();

	protected:
		inline const RenderVertex2D * getVerticesWM();

	protected:
		ResourceHolder<ResourceImage> m_resourceImage;

		bool m_blendAdd;
		bool m_solid;

		const RenderMaterialGroup * m_materialGroup;
		const RenderMaterial * m_material;

		mt::vec2f m_textureUVOffset;
		mt::vec2f m_textureUVScale;

        bool m_invalidateMaterial;

		size_t m_texturesNum;
		RenderTextureInterfacePtr m_textures[2];

		mt::vec3f m_verticesLocal[MENGINE_MESH_MAX_VERTEX];
		bool m_invalidateVerticesLocal;

		mt::vec2f m_uvLocal[MENGINE_MESH_MAX_VERTEX];
		bool m_invalidateUVLocal;

		RenderVertex2D m_verticesWM[MENGINE_MESH_MAX_VERTEX];
		bool m_invalidateVerticesWM;

		uint16 m_indices[MENGINE_MESH_MAX_INDECIES];

		size_t m_vertexCount;
		size_t m_indicesCount;

		bool m_invalidateVerticesColor;
    };
    //////////////////////////////////////////////////////////////////////////
    inline const RenderMaterial * Mesh::getMaterial()
    {
        if( m_invalidateMaterial == true )
        {
            this->updateMaterial();
        }

        return m_material;
    }
	//////////////////////////////////////////////////////////////////////////
	inline const RenderVertex2D * Mesh::getVerticesWM()
	{
		if( m_invalidateVerticesWM == true )
		{
			this->updateVerticesWM();
		}

		if( m_invalidateVerticesColor == true )
		{
			this->updateVerticesColor();
		}

		return m_verticesWM;
	}
}
