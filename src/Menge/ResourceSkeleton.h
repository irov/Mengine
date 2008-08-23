#	pragma once

#	include <vector>

#	include "ResourceReference.h"

namespace Menge
{
	typedef std::vector<String> TBoneNames;
	typedef std::vector<String> TAnimationNames;
	
	class ResourceSkeleton
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceSkeleton )

	public:
		ResourceSkeleton( const ResourceFactoryParam & _params );

	public:
		const TBoneNames & getBoneNames() const; 
		const TAnimationNames & getAnimationNames() const;

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _release() override;

	private:
		TBoneNames m_boneNames;

		TAnimationNames m_animationNames;

		void addBoneName( const String & _bone );
		void addAnimationName( const String & _animation );

		void loaderBone_( XmlElement * _xml );
		void loaderAnimation_( XmlElement * _xml );
	};
}