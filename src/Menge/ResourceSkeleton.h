#	pragma once

#	include <vector>

#	include "ResourceReference.h"

namespace Menge
{
	typedef std::vector<std::string> TBoneNames;
		
	class ResourceSkeleton
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceSkeleton )

	public:
		ResourceSkeleton( const std::string & _name );

	public:
		const	TBoneNames & getBoneNames() const; 

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _release() override;

	private:
		TBoneNames m_boneNames;

		void addBoneName( const std::string & _bone );
	};
}