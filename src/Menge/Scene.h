#	pragma once

#	include "NodeCore.h"
#	include "NodeChildren.h"

#	include "Scriptable.h"

#	include "Layer.h"

#	include "InputHandler.h"

#	include "InputHandler.h"

#	include "math\vec3.h"

namespace Menge
{
	class Camera2D;

	class Actor;
	class Light;
	class Entity3d;
	class Camera3D;
	class SceneNode3D;

	class Scene
		: public NodeCore
		, public NodeRenderable
		, public NodeChildren<Layer>
		, public InputHandler
	{
	public:
		Scene();
		~Scene();

	public:
		void layerAppend( const std::string & _layer, Node * _node );

		void setMainLayer( Layer * _layer );
		Layer * getMainLayer();

		Node * getNode( const std::string & _name );

	public:
		void setParentScene( Scene * _scene );
		bool isSubScene() const;

	public:
		const mt::vec2f & getLayerSize( const std::string & _name );

	public:
		void loader( XmlElement *_xml) override;
		void loaderScene_( XmlElement * _xml );
		void loaderEntities_( XmlElement * _xml );
		void loaderLights_( XmlElement * _xml );
		void loaderCameras_( XmlElement * _xml );

	public:
		bool handleKeyEvent( size_t _key, size_t _char, bool _isDown ) override;
		bool handleMouseButtonEvent( size_t _button, bool _isDown ) override;
		bool handleMouseMove( int _x, int _y, int _whell ) override;

	protected:
		bool _activate() override;
		void _deactivate() override;

		void _update( float _timing ) override;

		void _addChildren( Layer * _layer ) override;

	public:

		void addEntity( Entity3d * _entity );
		Entity3d * getEntity( const std::string & _name );

		void addLight( Light * _light );

		void addCamera( Camera3D * _camera );
		Camera3D * getCamera( const std::string & _name );

		void actorAppend( SceneNode3D * _node );

	private:

		bool m_isSubScene;
		Layer * m_mainLayer;

		mt::vec2f m_offsetPosition;

		mt::vec3f m_g;
		float m_restitution;
		float m_staticFriction; 
		float m_dynamicFriction;

		typedef std::map<std::string,Entity3d* > TMapEntity;
		typedef std::map<std::string,Light* > TMapLight;
		typedef std::map<std::string,Camera3D* > TMapCamera;

		TMapEntity	m_mapEntities;
		TMapLight	m_mapLights;
		TMapCamera	m_mapCameras;

		typedef std::list<SceneNode3D*> TListActors;

		TListActors	m_listActors;

		void activateLights_();
		void activateCameras_();
		void setPhysicParams_();
	};
}