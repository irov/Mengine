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
	void Skeleton::attachEntityToBone( const std::string & _bone, DiscreteEntity * _entity )
	{
		assert(0);
//		m_interface->attachEntityToBone(_bone, _entity->get());
	}
	//////////////////////////////////////////////////////////////////////////
}