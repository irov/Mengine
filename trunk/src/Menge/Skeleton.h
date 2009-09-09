#	include <string>

class SkeletonInterface;
class EntityInterface;

namespace Menge
{
	class DiscreteEntity;

	class Skeleton
	{
	public:
		Skeleton(SkeletonInterface * _skeleton);
	public:
		//void enableAnimation( const std::string & _anim, bool _enable );
		//void advanceAnimation( const std::string & _anim, float _timeDelta );
		//void advanceAllAnimations( float _timeDelta );
		//void setAnimationWeight( const std::string & _anim, float _weight );
		void attachEntityToBone( const std::string & _bone, DiscreteEntity * _entity );
	private://
		SkeletonInterface * m_interface;
	};
}
