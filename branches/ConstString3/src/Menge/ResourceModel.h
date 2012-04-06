//#	pragma once
//
//#	include "ResourceReference.h"
//#	include "Interface/RenderSystemInterface.h"
//#	include "Vertex.h"
//
//#	include <vector>
//
//namespace Menge
//{
//	class ResourceImageDefault;
//	
//	struct RenderMaterial;
//
//	struct AnimationMesh
//	{
//		AnimationMesh()
//			: resourceImage(0)
//			, in(0.f)
//			, out(0.f)
//			, verticesSize(0)
//			, ibHandle(0)
//		{
//		}
//
//		ConstString name;
//
//		ConstString textureName;
//		ResourceImageDefault * resourceImage;
//
//		const RenderTextureInterface * texture;
//
//		float in;
//		float out;
//
//		struct FrameMesh
//		{
//			TVectorVertex2D vertices;
//		};
//
//		typedef std::vector<FrameMesh> TVectorFrameMesh;
//		TVectorFrameMesh frameMeshes;
//
//		TVectorVertex2D::size_type verticesSize;
//
//		TVectorIndices indices;
//		IBHandle ibHandle;
//	};
//
//	typedef std::vector<AnimationMesh> TVectorAnimationMesh;
//
//	class ResourceModel
//		: public ResourceReference
//	{
//		RESOURCE_DECLARE( ResourceModel )
//
//	public:
//		ResourceModel();
//
//	public:
//		const TVectorAnimationMesh & getAnimationMeshes() const;
//		const RenderMaterial * getMaterial() const;
//
//		bool getFrameMeshFirst( const AnimationMesh & _am, AnimationMesh::FrameMesh & _frame );
//		bool getFrameMeshLast( const AnimationMesh & _am, AnimationMesh::FrameMesh & _frame );
//
//		bool getFrameMesh( const AnimationMesh & _am, float _timing, AnimationMesh::FrameMesh & _frame ) const;
//
//	protected:
//		void loader( BinParser * _parser ) override;
//		void loaderAnimationMesh_( BinParser * _parser, AnimationMesh & _mesh );
//		void loaderVertices_( BinParser * _parser, TVectorVertex2D & _vertices );
//
//	protected:
//		bool _compile() override;
//		void _release() override;
//
//	protected:
//		String m_filename;
//		ConstString m_meshName;
//
//		TVectorAnimationMesh m_animationMeshes;
//
//		float m_duration;
//		const RenderMaterial * m_material;
//		//std::string m_skeletonName;
//		//std::string m_materialName;
//	};
//}
