#	pragma once

#	include <vector>

#	include "ResourceReference.h"

namespace Menge
{
	typedef std::vector<std::string> TBoneNames;
	typedef std::vector<std::string> TAnimationNames;
	
	class ResourceSkeleton
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceSkeleton )

	public:
		ResourceSkeleton( const std::string & _name );

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

		void addBoneName( const std::string & _bone );
		void addAnimationName( const std::string & _animation );

		void loaderBone_( XmlElement * _xml );
		void loaderAnimation_( XmlElement * _xml );
	};
}