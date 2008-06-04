//#	include "DiscreteEntity.h"
//#	include "Interface/RenderSystemInterface.h"
//#	include "RenderEngine.h"
//#	include "XmlEngine.h"
//#	include "LogEngine.h"
//#	include "SceneNode3D.h"
//#	include "Skeleton.h"
//
//#	include "ResourceManager.h"
//#	include "ResourceMesh.h"
//#	include "ResourceSkeleton.h"
//#	include "ObjectImplement.h"
//
//namespace Menge
//{
//	//////////////////////////////////////////////////////////////////////////
//	OBJECT_IMPLEMENT(DiscreteEntity);
//	//////////////////////////////////////////////////////////////////////////
//	DiscreteEntity::DiscreteEntity( )
//		: m_resourceMesh(0)
//		, m_resourceSkeleton(0)
//		, m_castShadows(false)
//		, m_receiveShadows(false)
//		, m_skeleton(0)
//		, m_parent(0)
//		, m_entInterface( 0 )
//	{
//	}
//	//////////////////////////////////////////////////////////////////////////
//	DiscreteEntity::~DiscreteEntity()
//	{
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void DiscreteEntity::loader( XmlElement * _xml )
//	{
//		SceneNode3D::loader(_xml);
//
//		XML_SWITCH_NODE( _xml )
//		{
//			XML_CASE_ATTRIBUTE_NODE( "ResourceMesh", "Name", m_resourcenameMesh );
//			XML_CASE_ATTRIBUTE_NODE( "ResourceSkeleton", "Name", m_resourcenameSkeleton );
//			XML_CASE_ATTRIBUTE_NODE( "CastShadows", "Value", m_castShadows );
//			XML_CASE_ATTRIBUTE_NODE( "ReceiveShadows", "Value", m_receiveShadows );
//		}
//	}
//	//////////////////////////////////////////////////////////////////////////
//	bool DiscreteEntity::_activate()
//	{
//		if( SceneNode3D::_activate() == false )
//		{
//			return false;
//		}
//
//		return true;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void DiscreteEntity::_update( float _timing )
//	{
//	//	float min[3];
//	//float max[3];
//	//	m_entInterface->getAABB(min,max);
//	//	printf("min x = %f, y = %f, z = %f \n",min[0],min[1],min[2]);
//		_updateAnimation(_timing);
//	}
//	//////////////////////////////////////////////////////////////////////////
//	bool DiscreteEntity::_compile()
//	{
//		if( SceneNode3D::_compile() == false )
//		{
//			return false;
//		}
//
//		m_resourceMesh = 
//			Holder<ResourceManager>::hostage()
//			->getResourceT<ResourceMesh>( m_resourcenameMesh );
//
//		if( m_resourceMesh == NULL )
//		{
//			MENGE_LOG( "Error: DiscreteEntity can't open resource file '%s'\n", m_resourcenameMesh.c_str() );
//			return false;
//		}
//
//		/*m_resourceSkeleton = 
//			Holder<ResourceManager>::hostage()
//			->getResourceT<ResourceSkeleton>( m_resourcenameSkeleton );
//
//		if( m_resourceSkeleton != 0 )
//		{
//			const TBoneNames & boneNames = m_resourceSkeleton->getBoneNames();
//
//			for each( std::string boneName in boneNames )
//			{
//				Bone * bone = new Bone( this, boneName );
//				bone->setName( boneName );
//				this->addChildren( bone );
//			}
//		}*/
//
//		const std::string & mesh = m_resourceMesh->getMeshName();
//
//		const std::string & name = this->getName();
//
//		m_entInterface = Holder<RenderEngine>::hostage()->createEntity(name, mesh);
//
//		m_entInterface->setCastsShadow(m_castShadows);
//
//		SkeletonInterface * _interface = m_entInterface->getSkeleton();
//
//		if(_interface != NULL)
//		{
//			m_skeleton = new Skeleton(_interface);
//		}
//
//		m_interface->attachEntity( m_entInterface );
//		
//		return true;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void DiscreteEntity::_release()
//	{
//		SceneNode3D::_release();
//
//		delete m_skeleton;
//		Holder<RenderEngine>::hostage()->releaseEntity( m_entInterface );
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void DiscreteEntity::_deactivate()
//	{
//		SceneNode3D::_deactivate();
//	}
//	//////////////////////////////////////////////////////////////////////////
//	Skeleton * DiscreteEntity::getSkeleton() const
//	{
//		return m_skeleton;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void DiscreteEntity::setCastsShadow( bool _castsShadow )
//	{
//		return m_entInterface->setCastsShadow( _castsShadow );
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void DiscreteEntity::setVisible( bool _visible )
//	{
//		return m_entInterface->setVisible( _visible );
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void DiscreteEntity::setMaterial( const std::string & _material )
//	{
//		return m_entInterface->setMaterial( _material.c_str() );
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void DiscreteEntity::setSubEntityMaterial( const std::string & _subEntity, const std::string & _material )
//	{
//		return m_entInterface->setSubEntityMaterial( _subEntity.c_str(), _material.c_str() );
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void DiscreteEntity::createRenderToTexture( const std::string& _renderCamera, int _width, int _height )
//	{
//		m_entInterface->createRenderToTexture( _renderCamera.c_str(), _width, _height );
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void DiscreteEntity::setAnimationEnabled( const std::string& _animName, bool _enable )
//	{
//		m_entInterface->setAnimationEnabled( _animName.c_str(), _enable );
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void DiscreteEntity::playAnimation( const std::string& _animName )
//	{
//		TStringVector::iterator it = std::find( m_playingAnimations.begin(), m_playingAnimations.end(), _animName );
//
//		if( it == m_playingAnimations.end() )
//		{
//			m_entInterface->setAnimationEnabled( _animName.c_str(), true );
//			m_playingAnimations.push_back( _animName );
//		}
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void DiscreteEntity::pauseAnimation( const std::string& _animName )
//	{
//		m_playingAnimations.erase( std::find( m_playingAnimations.begin(), m_playingAnimations.end(), _animName ) );
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void DiscreteEntity::stopAnimation( const std::string& _animName )
//	{
//		m_entInterface->setAnimationEnabled( _animName.c_str(), false );
//		m_entInterface->setAnimationTimePosition( _animName.c_str(), 0.0f );
//
//		m_playingAnimations.erase( 
//			std::remove( m_playingAnimations.begin(), m_playingAnimations.end(), _animName ),
//			m_playingAnimations.end() 
//			);
//
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void DiscreteEntity::stopAllAnimations()
//	{
//		for( TStringVector::iterator it = m_playingAnimations.begin(),
//			it_end = m_playingAnimations.end(); it != it_end; it++ )
//		{
//			m_entInterface->setAnimationEnabled( (*it).c_str(), false );
//		}
//
//		m_playingAnimations.clear();
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void DiscreteEntity::_updateAnimation( float _timing )
//	{
//		for( TStringVector::iterator it = m_playingAnimations.begin(),
//			it_end = m_playingAnimations.end(); it != it_end; it++ )
//		{
//			m_entInterface->animationAddTime( (*it).c_str(), _timing );
//		}
//	}
//}