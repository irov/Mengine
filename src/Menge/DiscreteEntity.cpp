#	include "DiscreteEntity.h"
#	include "Interface/RenderSystemInterface.h"
#	include "RenderEngine.h"
#	include "XmlEngine.h"
#	include "LogEngine.h"

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
	{
	}
	//////////////////////////////////////////////////////////////////////////
	DiscreteEntity::~DiscreteEntity()
	{
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

		const std::string & meshName = m_resourceMesh->getMeshName();

		m_interface = Holder<RenderEngine>::hostage()->createEntity(m_name, meshName);

		m_interface->setCastsShadow( m_castShadows );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DiscreteEntity::deactivate()
	{
		Holder<RenderEngine>::hostage()->releaseEntity( m_interface );
	}
	//////////////////////////////////////////////////////////////////////////
	SkeletonInterface * DiscreteEntity::getSkeleton() const
	{
		return m_interface->getSkeleton();
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