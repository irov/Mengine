#	pragma once	

#	include "SceneNode3D.h"

#	include "AnimationObject.h"

namespace	Menge
{
	class AnimationBone : public SceneNode3D
	{
	public:
		AnimationBone(AnimationObject * _animObj, const std::string& _boneName);
		const mt::mat4f & getWorldMatrix();
	private:
		int m_index;
		AnimationObject * m_animObj;
	};
}