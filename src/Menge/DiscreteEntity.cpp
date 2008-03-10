#	include "DiscreteEntity.h"
#	include "Interface/RenderSystemInterface.h"
#	include "RenderEngine.h"
#	include "XmlEngine.h"
#	include "LogEngine.h"
#	include "SceneNode3D.h"
#	include "Skeleton.h"

#	include "ResourceManager.h"
#	include "ResourceMesh.h"
#	include "ResourceSkeleton.h"
#	include "ObjectImplement.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(DiscreteEntity);
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
//	DiscreteEntity::DiscreteEntity( const std::string & _name )
	DiscreteEntity::DiscreteEntity( )
		: m_resourceMesh(0)
		, m_resourceSkeleton(0)
		, m_castShadows(false)
		, m_receiveShadows(false)
		, m_skeleton(0)
		, m_parent(0)
		, m_entInterface( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	DiscreteEntity::~DiscreteEntity()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SceneNode3D * DiscreteEntity::getParentNode() const
	{
		return m_parent;
	}
	//////////////////////////////////////////////////////////////////////////
	void DiscreteEntity::setParentNode( SceneNode3D * _node ) 
	{
		m_parent = _node;
	}
	//////////////////////////////////////////////////////////////////////////
	void DiscreteEntity::loader( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "ResourceMesh", "Name", m_resourcenameMesh );
			XML_CASE_ATTRIBUTE_NODE( "ResourceSkeleton", "Name", m_resourcenameSkeleton );
			XML_CASE_ATTRIBUTE_NODE( "CastShadows", "Value", m_castShadows );
			XML_CASE_ATTRIBUTE_NODE( "ReceiveShadows", "Value", m_receiveShadows );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool DiscreteEntity::_activate()
	{
		if( SceneNode3D::_activate() == false )
		{
			return false;
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DiscreteEntity::update( float _timing )
	{
	/*	float min[3];
		float max[3];
		m_entInterface->getAABB(min,max);
		printf("min x = %f, y = %f, z = %f \n",min[0],min[1],min[2]);
*/
	}
	//////////////////////////////////////////////////////////////////////////
	bool DiscreteEntity::_compile()
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
			MENGE_LOG( "Error: DiscreteEntity can't open resource file '%s'\n", m_resourcenameMesh.c_str() );
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

		const std::string & mesh = m_resourceMesh->getMeshName();

		const std::string & name = this->getName();

		m_entInterface = Holder<RenderEngine>::hostage()->createEntity(name, mesh);

		m_entInterface->setCastsShadow(m_castShadows);

		SkeletonInterface * _interface = m_entInterface->getSkeleton();

		if(_interface != NULL)
		{
			m_skeleton = new Skeleton(_interface);
		}

		m_interface->attachEntity( m_entInterface );
		//this->getParentNode()->attachEntity(this);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DiscreteEntity::_release()
	{
		delete m_skeleton;
		Holder<RenderEngine>::hostage()->releaseEntity( m_entInterface );
	}
	//////////////////////////////////////////////////////////////////////////
	/*void DiscreteEntity::release()
	{

	}*/
	//////////////////////////////////////////////////////////////////////////
	void DiscreteEntity::_deactivate()
	{
		SceneNode3D::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	Skeleton * DiscreteEntity::getSkeleton() const
	{
		return m_skeleton;
	}
	//////////////////////////////////////////////////////////////////////////
	void DiscreteEntity::setCastsShadow( bool _castsShadow )
	{
		return m_entInterface->setCastsShadow( _castsShadow );
	}
	//////////////////////////////////////////////////////////////////////////
	void DiscreteEntity::setVisible( bool _visible )
	{
		return m_entInterface->setVisible( _visible );
	}
	//////////////////////////////////////////////////////////////////////////
	void DiscreteEntity::setMaterial( const std::string & _material )
	{
		return m_entInterface->setMaterial( _material.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void DiscreteEntity::setSubEntityMaterial( const std::string & _subEntity, const std::string & _material )
	{
		return m_entInterface->setSubEntityMaterial( _subEntity.c_str(), _material.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void DiscreteEntity::createRenderToTexture( const std::string& _renderCamera, int _width, int _height )
	{
		m_entInterface->createRenderToTexture( _renderCamera.c_str(), _width, _height );
	}
}