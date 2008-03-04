#	include "Layer3D.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "Scene.h"

#	include "Camera2D.h"

#	include "Player.h"

#	include "RenderEngine.h"

#	include "LogEngine.h"
#	include "Game.h"

#	include "NodeForeach.h"

#	include "Camera3d.h"
#	include "RigidBody3d.h"
#	include "Light.h"
#	include "Actor.h"
#	include "CapsuleController.h"
#	include "Application.h"
#	include "PhysicEngine.h"


namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Layer3D);
	//////////////////////////////////////////////////////////////////////////
	Layer3D::Layer3D()
		: m_g(0.0f, -9.8f, 0.0f)
		, m_restitution(0)
		, m_staticFriction(0)
		, m_dynamicFriction(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Layer3D::~Layer3D()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void Layer3D::update( float _timing )
	{
		NodeCore::update( _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer3D::handleKeyEvent( size_t _key, size_t _char, bool _isDown )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer3D::handleMouseButtonEvent( size_t _button, bool _isDown )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer3D::handleMouseMove( int _x, int _y, int _whell )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	SceneNode3D * Layer3D::getNode(const std::string & _name )
	{
		SceneNode3D * node = getChildrenT<SceneNode3D>( _name, true );
		return node;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer3D::loader( XmlElement * _xml )
	{
		NodeCore::loader(_xml);
		Layer::loader(_xml);

		std::string name;

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "Gravity", "Value", m_g );
			XML_CASE_ATTRIBUTE_NODE( "Restitution", "Value", m_restitution );
			XML_CASE_ATTRIBUTE_NODE( "StaticFriction", "Value", m_staticFriction );
			XML_CASE_ATTRIBUTE_NODE( "DynamicFriction", "Value", m_dynamicFriction );
		
			XML_CASE_NODE("RigidBodies")
			{
				XML_PARSE_ELEMENT( this, &Layer3D::loaderRigidBodies_ );
			}

			XML_CASE_NODE("Controllers")
			{
				XML_PARSE_ELEMENT( this, &Layer3D::loaderControllers_ );
			}
		}
		XML_END_NODE()
		{
			callMethod( "onLoader", "()" );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer3D::_activate()
	{
		setPhysicParams_();
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer3D::_deactivate()
	{
		for( TMapRigidBody::iterator
			it = m_mapRigidBodies.begin(),
			it_end = m_mapRigidBodies.end();
		it != it_end;
		++it)
		{
			it->second->deactivate();
		}

		callMethod( "onDeactivate", "() ");
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer3D::_release()
	{
		for( TMapRigidBody::iterator
			it = m_mapRigidBodies.begin(),
			it_end = m_mapRigidBodies.end();
		it != it_end;
		++it)
		{
			it->second->release();
			delete it->second;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer3D::_addChildren( SceneNode3D * _node )
	{
		_node->attachToRootNode();
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer3D::loaderRigidBodies_( XmlElement * _xml )
	{
		std::string name;

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("RigidBody")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", name );
				}

				RigidBody3D * body = new RigidBody3D();

				body->setName( name );
				body->setType( "RigidBody3D" );

				addRigidBody( body );

				XML_PARSE_ELEMENT( body, &RigidBody3D::loader );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer3D::loaderControllers_( XmlElement * _xml )
	{
		std::string name;

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("Controller")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", name );
				}

				CapsuleController * capsule = new CapsuleController();

				capsule->setName( name );
				capsule->setType( "CapsuleController" );

				addController( capsule );

				XML_PARSE_ELEMENT( capsule, &CapsuleController::loader );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer3D::addRigidBody( RigidBody3D * _rigidBody )
	{
		const std::string & name = _rigidBody->getName();

		TMapRigidBody::iterator it_find = m_mapRigidBodies.find( name );

		if( it_find == m_mapRigidBodies.end() )
		{
			m_mapRigidBodies.insert( std::make_pair( name, _rigidBody ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer3D::addController( CapsuleController * _capsule )
	{
		const std::string & name = _capsule->getName();

		TMapControllers::iterator it_find = m_mapControllers.find( name );

		if( it_find == m_mapControllers.end() )
		{
			m_mapControllers.insert( std::make_pair( name, _capsule ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	CapsuleController * Layer3D::getController( const std::string & _name )
	{
		TMapControllers::const_iterator it_find = m_mapControllers.find( _name );

		if( it_find == m_mapControllers.end() )
		{
			return NULL;
		}

		CapsuleController * cap = it_find->second;

		cap->activate();

		return cap;
	}
	//////////////////////////////////////////////////////////////////////////
	RigidBody3D * Layer3D::getRigidBody( const std::string & _name )
	{
		TMapRigidBody::const_iterator it_find = m_mapRigidBodies.find( _name );

		if( it_find == m_mapRigidBodies.end() )
		{
			return NULL;
		}

		RigidBody3D * body = it_find->second;

		body->activate();

		return body;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer3D::setPhysicParams_()
	{
		bool usePhysic = Holder<Application>::hostage()->usePhysic();

		if( usePhysic == true )
		{
			Holder<PhysicEngine>::hostage()->setGravity( m_g );
			Holder<PhysicEngine>::hostage()->setRestitution( m_restitution );
			Holder<PhysicEngine>::hostage()->setStaticFriction( m_staticFriction );
			Holder<PhysicEngine>::hostage()->setDynamicFriction( m_dynamicFriction );		
		}
	}
}