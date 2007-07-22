#	pragma once
#	include <cal3d\cal3d.h>
#	include <cal3d\animcallback.h>
#	include <string>

#	include "AnimationObject.h"

namespace Menge
{
	class AnimationCallback : public CalAnimationCallback
	{
	public:
		AnimationCallback(AnimationObject * _animObj, const std::string & _name, TUpdateCallback _updateCallback, TCompleteCallback _completeCallback, void * _userData);

	public:
		void AnimationUpdate(float anim_time, CalModel * model, void * userData) override;
		void AnimationComplete(CalModel * model, void * userData) override;

	private:
		AnimationObject	* m_animObj;
		TUpdateCallback	m_updateCallback;
		TCompleteCallback m_completeCallback;
		std::string	m_name;
		void * m_userData; 
	};
};