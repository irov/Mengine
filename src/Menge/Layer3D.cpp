#	include "Layer3D.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "Scene.h"

#	include "Camera2D.h"

#	include "Player.h"

#	include "RenderEngine.h"

#	include "LogEngine.h"
#	include "Game.h"

//#	include "NodeForeach.h"

//#	include "Camera3d.h"
//#	include "RigidBody3d.h"
//#	include "Light.h"
//#	include "Actor.h"
#	include "CapsuleController.h"
#	include "Application.h"
#	include "PhysicEngine.h"

#	include "VisitorAdapter.h"

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

	//////////////////////////////////////////////////////////////////////////
	bool Layer3D::handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer3D::handleMouseButtonEvent( unsigned int _button, bool _isDown )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer3D::handleMouseMove( float _x, float _y, int _whell )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	void Layer3D::loader( XmlElement * _xml )
	{
		Node::loader(_xml);
	//	Layer::loader(_xml);

		std::string name;

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "Gravity", "Value", m_g );
			XML_CASE_ATTRIBUTE_NODE( "Restitution", "Value", m_restitution );
			XML_CASE_ATTRIBUTE_NODE( "StaticFriction", "Value", m_staticFriction );
			XML_CASE_ATTRIBUTE_NODE( "DynamicFriction", "Value", m_dynamicFriction );
		
			XML_CASE_NODE("Controllers")
			{
				XML_PARSE_ELEMENT( this, &Layer3D::loaderControllers_ );
			}
		}
		//XML_END_NODE()
		//{
		//	callMethod( "onLoader", "()" );
		//}
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
		//callMethod( "onDeactivate", "() ");
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer3D::_release()
	{
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	void Layer3D::loaderControllers_( XmlElement * _xml )
	{
		std::string name;

		XML_SWITCH_NODE( _xml )
		{
			
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer3D::addController( CapsuleController * _capsule )
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	CapsuleController * Layer3D::getController( const std::string & _name )
	{

		return NULL;
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
	/////////////////////////////////////////////////////////////////////////
	class VisitorRenderLayer3D
		: public VisitorAdapter<VisitorRenderLayer3D>
	{
	public:
		VisitorRenderLayer3D( const Viewport & _viewport, bool _enableDebug )
			: m_viewport(_viewport)
			, m_enableDebug( _enableDebug )
		{
		}

	public:
		void visit( Node * _node )
		{				
			if( _node->isRenderable() == true )
			{
				_node->_render( m_viewport, m_enableDebug );

				_node->visitChildren( this );
			}

		}

		void visit( Layer * _layer )
		{
			_layer->render( m_viewport, m_enableDebug );
		}

	protected:
		Viewport m_viewport;
		bool m_enableDebug;
	};
	//////////////////////////////////////////////////////////////////////////
	void Layer3D::render( const Viewport & _viewport, bool _enableDebug )
	{
		Layer::_render( _viewport, _enableDebug );
		RenderEngine* engine = Holder<RenderEngine>::hostage();
		engine->beginLayer3D();
		
		VisitorRenderLayer3D visitorRender( _viewport, _enableDebug );

		visitChildren( &visitorRender );

		Holder<Player>::hostage()->getRenderCamera2D()->changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
}