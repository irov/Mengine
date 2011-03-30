#	pragma once

#	include "ResourceReference.h"
#	include "Vertex.h"

#	include <vector>

namespace Menge
{
	class ResourceImageDefault;

	struct AnimationMesh
	{
		AnimationMesh()
			: resourceImage(0)
			, frameRate(0.f)
			, frameBegin(0.f)
			, frameEnd(0.f)
			, frameStep(0.f)
		{
		}

		ConstString name;

		ConstString texture;
		ResourceImageDefault * resourceImage;

		float frameRate;
		float frameBegin;
		float frameEnd;
		float frameStep;

		struct FrameMesh
		{
			float frame;
			TVectorVertex2D vertecies;
		};

		typedef std::vector<FrameMesh> TVectorFrameMesh;
		TVectorFrameMesh frameMeshes;

		TVectorIndices indicies;
	};

	typedef std::vector<AnimationMesh> TVectorAnimationMesh;

	class ResourceModel
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceModel )

	public:
		ResourceModel();

	protected:
		void loader( BinParser * _parser ) override;
		void loaderAnimationMesh_( BinParser * _parser, AnimationMesh & _mesh );
		void loaderVertices_( BinParser * _parser, TVectorVertex2D & _vertices );

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		String m_filename;
		ConstString m_meshName;

		TVectorAnimationMesh m_animationMeshes;
		//std::string m_skeletonName;
		//std::string m_materialName;
	};
}
