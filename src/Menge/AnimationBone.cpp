#	include "AnimationBone.h"

namespace	Menge
{
	AnimationBone::AnimationBone(AnimationObject * _animObj, const std::string& _boneName)
		: m_animObj(_animObj)
	{
		m_index = _animObj->getBoneIndex(_boneName);
	}

	const mt::mat4f & AnimationBone::getWorldMatrix()
	{
		CalBone * bone = m_animObj->m_calModel->getSkeleton()->getBone(m_index);

		const CalVector & position = bone->getTranslationAbsolute();
		const CalQuaternion & orient = bone->getRotationAbsolute();

		mt::vec3f pos(position.x,position.y,position.z);
		mt::quatf q(orient.w,orient.x,orient.y,orient.z);

		mt::mat4f worldMatrix = mt::qpos_to_rot_m4(q,pos);

		const mt::mat4f & mat = m_animObj->getWorldMatrix();

		m_worldMatrix = worldMatrix * mat;

		_changePivot(); // <----

		return m_worldMatrix;
	}
}