#	include "Entity3d.h"
#	include "ObjectImplement.h"

#	include "ScriptEngine.h"
#	include "RenderEngine.h"
#	include "LogEngine.h"

#	include "ResourceManager.h"
#	include "ResourceMesh.h"
#	include "ResourceSkeleton.h"

#	include "Bone.h"

#	include "XmlEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Entity3d )
	//////////////////////////////////////////////////////////////////////////
	Entity3d::Entity3d()
		: m_resourceMesh(0)
		, m_resourceSkeleton(0)
		, m_entityInterface(0)
		, m_isGrounded(false)
		, m_scale(1.0f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Entity3d::~Entity3d()
	{}
	//////////////////////////////////////////////////////////////////////////
	void Entity3d::_update( float _timing )
	{
		m_entityInterface->update( _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::quatf & Entity3d::getBoneWorldOrient( const std::string& _name )
	{
		return *(mt::quatf*)m_entityInterface->getBoneOrientation( _name.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Entity3d::getBoneWorldPosition( const std::string& _name ) 
	{
		return *(mt::vec3f*)m_entityInterface->getBonePosition( _name.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity3d::loader( XmlElement * _xml )
	{
		NodeCore::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "ResourceMesh", "Name", m_resourcenameMesh );
			XML_CASE_ATTRIBUTE_NODE( "ResourceSkeleton", "Name", m_resourcenameSkeleton );
			XML_CASE_ATTRIBUTE_NODE( "IsGrounded", "Value", m_isGrounded );
			XML_CASE_ATTRIBUTE_NODE( "Scale", "Value", m_scale );
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

		m_entityInterface = Holder<RenderEngine>::hostage()->createEntity(entityName, meshName);

		float scale[3] = {m_scale,m_scale,m_scale};

	//	m_interface->setScale(scale);

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
		m_entityInterface->play( _name.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity3d::play2( const std::string& _name1, float _weight1, const std::string& _name2, float _weight2 )
	{
		m_entityInterface->play( _name1.c_str(), _weight1, _name2.c_str(), _weight2 );
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity3d::stop()
	{
		m_entityInterface->stop();
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity3d::setLooped( bool _looped )
	{
		m_entityInterface->setLooped( _looped );
	}
	//////////////////////////////////////////////////////////////////////////
}