#	pragma once	

#	include "SceneNode3D.h"

#	include "AnimationObject.h"

class Bone : public SceneNode3D
{
public:
	Bone(AnimationObject * _animObj, const std::string& _boneName)
		: m_animObj(_animObj)
	{
		m_index = _animObj->getBoneIndex(_boneName);
	}

private:
	int m_index;
	AnimationObject * m_animObj;
};