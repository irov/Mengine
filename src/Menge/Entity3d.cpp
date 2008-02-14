#	include "Entity3d.h"
#	include "ObjectImplement.h"

#	include "ScriptEngine.h"
#	include "RenderEngine.h"
#	include "LogEngine.h"

#	include "ResourceManager.h"
#	include "ResourceMesh.h"
#	include "ResourceSkeleton.h"

#	include "XmlEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Entity3d )
	//////////////////////////////////////////////////////////////////////////
	Entity3d::Entity3d()
		: m_resourceMesh(0)
		, m_resourceSkeleton(0)
		, m_interface(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Entity3d::~Entity3d()
	{
		Holder<RenderEngine>::hostage()->releaseEntity( m_interface );
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity3d::_update( float _timing )
	{
		if(m_animName.empty()==false)
		{
			SkeletonInterface * skeleton = m_interface->getSkeleton();
			skeleton->advanceAnimation(m_animName, _timing / 1000.f);
		}
	//	m_interface->update( _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity3d::loader( XmlElement * _xml )
	{
		NodeCore::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "ResourceMesh", "Name", m_resourcenameMesh );
			XML_CASE_ATTRIBUTE_NODE( "ResourceSkeleton", "Name", m_resourcenameSkeleton );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity3d::_activate()
	{
		if( NodeCore::_activate() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity3d::_deactivate()
	{
		NodeCore::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity3d::_compile()
	{
		if( NodeCore::_compile() == false )
		{
			return false;
		}

		m_resourceMesh = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceMesh>( m_resourcenameMesh );

		if( m_resourceMesh == NULL )
		{
			MENGE_LOG( "Error: Emitter can't open resource file '%s'\n", m_resourcenameMesh.c_str() );
			return false;
		}

		/*m_resourceSkeleton = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceSkeleton>( m_resourcenameSkeleton );

		if( m_resourceSkeleton != 0 )
		{
			const TBoneNames & boneNames = m_resourceSkeleton->getBoneNames();

			for each( std::string boneName in boneNames )
			{
				Bone * bone = new Bone( this, boneName );
				bone->setName( boneName );
				this->addChildren( bone );
			}
		}*/

		const std::string & entityName = this->getName();
		const std::string & meshName = m_resourceMesh->getMeshName();

		m_interface = Holder<RenderEngine>::hostage()->createEntity(entityName, meshName);

	
//		m_interface->attachEntity( m_entityInterface );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity3d::_release()
	{
		NodeCore::_release();
		//
		
		Holder<ResourceManager>::hostage()
			->releaseResource( m_resourceMesh );

		m_resourceMesh = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity3d::play( const std::string& _name )
	{
		SkeletonInterface * skeleton = m_interface->getSkeleton();
		skeleton->enableAnimation( _name, true );
		m_animName = _name;
	//	m_interface->play( _name.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity3d::play2( const std::string& _name1, float _weight1, const std::string& _name2, float _weight2 )
	{
	//	m_interface->play( _name1.c_str(), _weight1, _name2.c_str(), _weight2 );
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity3d::stop()
	{
	//	m_interface->stop();
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity3d::setLooped( bool _looped )
	{
	//	m_interface->setLooped( _looped );
	}
	//////////////////////////////////////////////////////////////////////////
}