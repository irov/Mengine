#	include "Bone.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Bone::Bone(RenderMesh3D * _mesh, const std::string & _name )
		: m_mesh(_mesh)
		, m_boneName(_name)
		, m_worldOrient(1.0f, 0.0f, 0.0f, 0.0f)
		, m_worldPosition(0.0f, 0.0f, 0.0f)
	{}
	//////////////////////////////////////////////////////////////////////////
	const mt::quatf & Bone::getWorldOrient()
	{
		const mt::quatf & worldMatrix = m_mesh->getBoneWorldOrient(m_boneName);
		const mt::quatf & objWorldOrient = m_mesh->getWorldOrient();
		m_worldOrient = worldMatrix * objWorldOrient;
		return m_worldOrient;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Bone::getWorldPosition()
	{
		const mt::vec3f & worldPos = m_mesh->getBoneWorldPosition(m_boneName);
		const mt::vec3f & objWorldPos = m_mesh->getWorldPosition();
		m_worldPosition = worldPos + objWorldPos;
		return m_worldPosition;
	}
	//////////////////////////////////////////////////////////////////////////
}