#	include "RenderMesh3D.h"
#	include "ObjectImplement.h"

#	include "ScriptEngine.h"
#	include "RenderEngine.h"
#	include "PhysicEngine.h"
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
	, m_interfaceMesh(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RenderMesh3D::~RenderMesh3D()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh3D::_update( float _timing )
	{
		const mt::vec3f& pos = this->getWorldPosition();
		m_interface->setPosition(pos.x,pos.y,pos.z);

		mt::quatf q = this->getWorldOrient();
		m_interface->setDirection(q.w, q.x, q.y, q.z);
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::quatf & RenderMesh3D::getBoneWorldOrient( const std::string& _name )
	{
		m_interfaceMesh->getBoneOrientation( _name.c_str(), m_worldBoneOrient.w, m_worldBoneOrient.x, m_worldBoneOrient.y, m_worldBoneOrient.z );
		return m_worldBoneOrient;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & RenderMesh3D::getBoneWorldPosition( const std::string& _name ) 
	{
		m_interfaceMesh->getBonePos( _name.c_str(), m_worldBonePos.x, m_worldBonePos.y, m_worldBonePos.z );
		return m_worldBonePos;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh3D::loader( XmlElement * _xml )
	{
		SceneNode3D::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "ResourceMesh", "Name", m_resourcenameMesh );
			XML_CASE_ATTRIBUTE_NODE( "ResourceSkeleton", "Name", m_resourcenameSkeleton );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderMesh3D::_activate()
	{
		bool result = SceneNode3D::_activate();
		return result;
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

		m_interfaceMesh = m_resourceMesh->getMesh();

		if( m_interfaceMesh == 0 )
		{
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
		std::string meshName = m_interfaceMesh->getName();
		m_interface = Holder<RenderEngine>::hostage()->create3dEntity( entityName, meshName );

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
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh3D::play( const std::string& _name1, float _weight1, const std::string& _name2, float _weight2 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh3D::stop()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh3D::setLooped( bool _looped )
	{
	}
	//////////////////////////////////////////////////////////////////////////
}
