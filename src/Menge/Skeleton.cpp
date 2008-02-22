#	include "Skeleton.h"
#	include "DiscreteEntity.h"
#	include "Interface/RenderSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Skeleton::Skeleton(SkeletonInterface * _skeleton)
		: m_interface(_skeleton)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Skeleton::enableAnimation( const std::string & _anim, bool _enable )
	{
		m_interface->enableAnimation(_anim, _enable);
	}
	//////////////////////////////////////////////////////////////////////////
	void Skeleton::advanceAnimation( const std::string & _anim, float _timeDelta )
	{
		m_interface->advanceAnimation(_anim, _timeDelta);
	}
	//////////////////////////////////////////////////////////////////////////
	void Skeleton::advanceAllAnimations( float _timeDelta )
	{
		m_interface->advanceAllAnimations(_timeDelta);
	}
	//////////////////////////////////////////////////////////////////////////
	void Skeleton::setAnimationWeight( const std::string & _anim, float _weight )
	{
		m_interface->setAnimationWeight(_anim, _weight);
	}
	//////////////////////////////////////////////////////////////////////////
	void Skeleton::attachEntityToBone( const std::string & _bone, DiscreteEntity * _entity )
	{
		m_interface->attachEntityToBone(_bone, _entity->get());
	}
	//////////////////////////////////////////////////////////////////////////
}