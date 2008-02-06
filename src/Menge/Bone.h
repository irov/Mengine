/*#	pragma once	

#	include "RenderMesh3D.h"

namespace	Menge
{
	class Bone : public SceneNode3D
	{
	public:
		Bone( RenderMesh3D * _mesh, const std::string & _name );

	public:
		const mt::quatf & getWorldOrient();
		const mt::vec3f & getWorldPosition();

	private:
		RenderMesh3D * m_mesh;
		std::string m_boneName;

		mt::quatf m_worldOrient;
		mt::vec3f m_worldPosition;

		void render(){}
		bool isRenderable(){return false;}
	};
}*/