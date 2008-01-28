#	include "Bone.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Bone::Bone(RenderMesh3D * _mesh, const std::string & _name )
		: m_mesh(_mesh)
		, m_boneName(_name)
	{}
	//////////////////////////////////////////////////////////////////////////
	const mt::quatf & Bone::getWorldOrient()
	{
		assert(0);
		mt::quatf worldMatrix = m_mesh->getBoneWorldOrient(m_boneName);

		//const mt::quatf & objWorldOrient = m_mesh->getWorldOrient();

		//m_worldOrient = worldMatrix * objWorldOrient;

		//return m_worldOrient;
		return worldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Bone::getWorldPosition()
	{
		assert(0);
		mt::vec3f worldPos = m_mesh->getBoneWorldPosition(m_boneName);

	//	const mt::vec3f & objWorldPos = m_mesh->getWorldPosition();
//
		//m_worldPosition = worldPos + objWorldPos;

		//return m_worldPosition;
		return worldPos;
	}
	//////////////////////////////////////////////////////////////////////////
}