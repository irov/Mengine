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

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DiscreteEntity::DiscreteEntity( const std::string & _name )
		: m_name(_name)
		, m_resourceMesh(0)
		, m_resourceSkeleton(0)
		, m_castShadows(false)
		, m_receiveShadows(false)
		, m_skeleton(0)
		, m_parent(0)
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
			XML_CASE_ATTRIBUTE_NODE( "CastShadows", "Name", m_castShadows );
			XML_CASE_ATTRIBUTE_NODE( "ReceiveShadows", "Name", m_receiveShadows );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool DiscreteEntity::activate()
	{
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

		m_interface = Holder<RenderEngine>::hostage()->createEntity(m_name, mesh);

		m_interface->setCastsShadow(m_castShadows);

		SkeletonInterface * _interface = m_interface->getSkeleton();

		if(_interface != NULL)
		{
			m_skeleton = new Skeleton(_interface);
		}

		this->getParentNode()->attachEntity(this);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DiscreteEntity::deactivate()
	{
		delete m_skeleton;
		Holder<RenderEngine>::hostage()->releaseEntity( m_interface );
	}
	//////////////////////////////////////////////////////////////////////////
	Skeleton * DiscreteEntity::getSkeleton() const
	{
		return m_skeleton;
	}
	//////////////////////////////////////////////////////////////////////////
	void DiscreteEntity::setCastsShadow( bool _castsShadow )
	{
		return m_interface->setCastsShadow( _castsShadow );
	}
	//////////////////////////////////////////////////////////////////////////
	void DiscreteEntity::setVisible( bool _visible )
	{
		return m_interface->setVisible( _visible );
	}
	//////////////////////////////////////////////////////////////////////////
	void DiscreteEntity::setMaterial( const std::string & _material )
	{
		return m_interface->setMaterial( _material.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void DiscreteEntity::setSubEntityMaterial( const std::string & _subEntity, const std::string & _material )
	{
		return m_interface->setSubEntityMaterial( _subEntity.c_str(), _material.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
}