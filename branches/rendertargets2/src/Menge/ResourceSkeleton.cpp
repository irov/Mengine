#	include "ResourceSkeleton.h"

#	include "ResourceImplement.h"

#	include "PhysicEngine.h"

#	include "Logger/Logger.h"

#	include "XmlEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceSkeleton );
	//////////////////////////////////////////////////////////////////////////
	ResourceSkeleton::ResourceSkeleton()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSkeleton::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Bones" )
			{
				XML_PARSE_ELEMENT( this, &ResourceSkeleton::loaderBone_ );
			}
			XML_CASE_NODE( "Animations" )
			{
				XML_PARSE_ELEMENT( this, &ResourceSkeleton::loaderAnimation_ );
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
	const TAnimationNames & ResourceSkeleton::getAnimationNames() const
	{
		return m_animationNames;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSkeleton::addBoneName( const String & _bone )
	{
		m_boneNames.push_back(_bone);
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSkeleton::addAnimationName( const String & _animation )
	{
		m_animationNames.push_back(_animation);
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSkeleton::loaderBone_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Bone", "Name", &ResourceSkeleton::addBoneName );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSkeleton::loaderAnimation_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Animation", "Name", &ResourceSkeleton::addAnimationName );
		}		
	}
}
