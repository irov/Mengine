#	include "RenderMesh3D.h"
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
	OBJECT_IMPLEMENT( RenderMesh3D )
	//////////////////////////////////////////////////////////////////////////
	RenderMesh3D::RenderMesh3D()
	: m_resourceMesh(0)
	, m_resourceSkeleton(0)
	, m_entityInterface(0)
	, m_isGrounded(false)
	, m_scale(1.0f)
	{}
	//////////////////////////////////////////////////////////////////////////
	RenderMesh3D::~RenderMesh3D()
	{}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh3D::_update( float _timing )
	{
		if(m_isGrounded)
		{
			mt::vec3f pos = this->getWorldPosition();
			printf("%f %f %f\n",pos.x,pos.y,pos.z);
			float dist = Holder<RenderEngine>::hostage()->getQueryDistance("Level",pos,mt::vec3f(0,-1,0));
		//	pos.y-=dist;
		//	this->setLocalPosition(pos);
			printf("%f \n",dist);
		}

		m_entityInterface->update( _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::quatf & RenderMesh3D::getBoneWorldOrient( const std::string& _name )
	{
		return *(mt::quatf*)m_entityInterface->getBoneOrientation( _name.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & RenderMesh3D::getBoneWorldPosition( const std::string& _name ) 
	{
		return *(mt::vec3f*)m_entityInterface->getBonePosition( _name.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh3D::loader( XmlElement * _xml )
	{
		SceneNode3D::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "ResourceMesh", "Name", m_resourcenameMesh );
			XML_CASE_ATTRIBUTE_NODE( "ResourceSkeleton", "Name", m_resourcenameSkeleton );
			XML_CASE_ATTRIBUTE_NODE( "IsGrounded", "Value", m_isGrounded );
			XML_CASE_ATTRIBUTE_NODE( "Scale", "Value", m_scale );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderMesh3D::_activate()
	{
		if( SceneNode3D::_activate() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh3D::_deactivate()
	{
		SceneNode3D::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderMesh3D::_compile()
	{
		if( SceneNode3D::_compile() == false )
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

		m_resourceSkeleton = 
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
		}

		const std::string & entityName = this->getName();
		const std::string & meshName = m_resourceMesh->getMeshName();

		m_entityInterface = Holder<RenderEngine>::hostage()->createEntity(entityName, meshName);

		float scale[3] = {m_scale,m_scale,m_scale};

		m_interface->setScale(scale);

		m_interface->attachEntity( m_entityInterface );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh3D::_release()
	{
		SceneNode3D::_release();
		//
		
		Holder<ResourceManager>::hostage()
			->releaseResource( m_resourceMesh );

		m_resourceMesh = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh3D::play( const std::string& _name )
	{
		m_entityInterface->play( _name.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh3D::play2( const std::string& _name1, float _weight1, const std::string& _name2, float _weight2 )
	{
		m_entityInterface->play( _name1.c_str(), _weight1, _name2.c_str(), _weight2 );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh3D::stop()
	{
		m_entityInterface->stop();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh3D::setLooped( bool _looped )
	{
		m_entityInterface->setLooped( _looped );
	}
	//////////////////////////////////////////////////////////////////////////
}