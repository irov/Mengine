#	pragma once	

#	include "SceneNode3D.h"

#	include "AnimationObject.h"

namespace	Menge
{
	class AnimationBone : public SceneNode3D
	{
	public:
		AnimationBone( AnimationObject * _animObj, int _index );
		const mt::mat4f & getWorldMatrix() override;
		void _debugRender() override;
	private:
		int m_index;
		AnimationObject * m_animObj;
	};
}