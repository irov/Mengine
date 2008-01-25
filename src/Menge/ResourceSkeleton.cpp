#	include "ResourceSkeleton.h"

#	include "ResourceImplement.h"

#	include "PhysicEngine.h"

#	include "LogEngine.h"

#	include "XmlEngine.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceSkeleton )
	//////////////////////////////////////////////////////////////////////////
	ResourceSkeleton::ResourceSkeleton( const std::string & _name )
		: ResourceReference(_name)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSkeleton::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Bone" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE_MEMBER( "Name", &ResourceSkeleton::addBoneName );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSkeleton::_compile()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSkeleton::_release()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const TBoneNames & ResourceSkeleton::getBoneNames() const
	{
		return m_boneNames;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSkeleton::addBoneName( const std::string & _bone )
	{
		m_boneNames.push_back(_bone);
	}
}