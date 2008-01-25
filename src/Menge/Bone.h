#	pragma once	

#	include "RenderMesh3D.h"

namespace	Menge
{
	class Bone : public SceneNode3D
	{
	public:
		Bone( RenderMesh3D * _mesh, const std::string & _name );
		const mt::quatf & getWorldOrient() override;
		const mt::vec3f & getWorldPosition() override;

	private:
		RenderMesh3D * m_mesh;
		std::string m_boneName;
	};
}