#	include	"AnimationCallbacks.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AnimationCallback::AnimationCallback(AnimationObject * _animObj, const std::string & _name, UpdateCallback _updateCallback, CompleteCallback _completeCallback, void * _userData)
		: m_animObj(_animObj)
		, m_updateCallback(_updateCallback)
		, m_completeCallback(_completeCallback)
		, m_name(_name)
		, m_userData(_userData)
	{}
	//////////////////////////////////////////////////////////////////////////
	void AnimationCallback::AnimationUpdate(float anim_time, CalModel * model, void * userData)
	{
		if(m_animObj->isSimilarModel(model) && m_updateCallback != NULL)
		{
			m_updateCallback(m_animObj, m_name, anim_time, m_userData);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationCallback::AnimationComplete(CalModel * model, void * userData)
	{
		if(m_animObj->isSimilarModel(model) && m_completeCallback != NULL)
		{
			m_completeCallback(m_animObj, m_name, m_userData);
		}
	}
	//////////////////////////////////////////////////////////////////////////
}