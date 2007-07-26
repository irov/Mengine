#	include "AnimationBone.h"

#	include "math/box.h"

#	include "RenderEngine.h"

namespace	Menge
{
	AnimationBone::AnimationBone(AnimationObject * _animObj, int _index)
		: m_animObj(_animObj)
		, m_index(_index)
	{}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & AnimationBone::getWorldMatrix()
	{
		CalBone * bone = m_animObj->getBoneWithIndex(m_index);

		const CalVector & position = bone->getTranslationAbsolute();
		const CalQuaternion & orient = bone->getRotationAbsolute();

		mt::vec3f pos(position.x,position.y,position.z);
		mt::quatf q(orient.w,orient.x,orient.y,orient.z);

		mt::mat4f worldMatrix = mt::qpos_to_rot_m4(q,pos);

		const mt::mat4f & objWorldMatrix = m_animObj->getWorldMatrix();

		m_worldMatrix = worldMatrix * objWorldMatrix;

		return m_worldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationBone::_debugRender()
	{
		const mt::mat4f & matrix = getWorldMatrix();
		mt::vec3f points = matrix.v3.v3;
		mt::vec3f p(points[0], points[1], points[2]);
		mt::boxf box;
		mt::set_box_from_center_and_extent(box, p, mt::vec3f(5.0f, 5.0f, 5.0f));
	
		RenderEngine * renderEng = Holder<RenderEngine>::hostage();

		mt::mat4f id;
		mt::ident_m4(id);
		renderEng->setWorldMatrix(id);

		renderEng->drawBox(box.MinEdge,box.MaxEdge, 0xaaff00ff);
	}
}