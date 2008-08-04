#	pragma once

#	include "Layer.h"
#	include "SceneNode3D.h"
#	include "Viewport.h"

namespace Menge
{
	class Light;
	class Entity3d;
	class CapsuleController;

	class Layer3D
		: public Layer
		, public Allocator3D
		//: public SceneNode3D
	{
		OBJECT_DECLARE(Layer3D);

	public:
		Layer3D();
		virtual ~Layer3D();

	public:
		void addController( CapsuleController * _capsule );
		CapsuleController * getController( const std::string & _name );

		//SceneNode3D * getNode( const std::string & _name );

	public:
		void loader( XmlElement * _xml ) override;
		void loaderControllers_( XmlElement * _xml );

		bool handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, int _whell ) override;

	//public:
		//void update( float _timing ) override;
	
		//void _render( const Viewport & _viewport, bool _enableDebug ) override;
		void render( unsigned int _debugMask ) override;

	protected:

		bool _activate() override;
		void _deactivate() override;
		void _release() override;

		//void _addChildren( SceneNode3D * _node ) override;

		//bool _renderBegin();

	private:

		mt::vec3f m_g;
		float m_restitution;
		float m_staticFriction; 
		float m_dynamicFriction;

		typedef std::map<std::string,CapsuleController* > TMapControllers;

		TMapControllers m_mapControllers;

		void setPhysicParams_();

	private:
		//void render( bool _enableDebug ){ NodeRenderable::render( _enableDebug ); }
		bool isRenderable(){ return true; }
		Viewport m_viewport;
	};
}