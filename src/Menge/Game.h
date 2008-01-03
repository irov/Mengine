#	pragma once

#	include "ResourceManager.h"

#	include "InputHandler.h"
#	include "Eventable.h"
#	include "math/vec2.h"

#	include <list>
#	include <map>

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

class XmlElement;

namespace Menge
{
	class Player;

	class Scene;
	class Arrow;

	class Amplifier;

	class Game
		: public InputHandler
		, public Eventable
		, public ResourceManagerListener
	{
	public:
		Game();
		~Game();

	public:
		void update( float _timing );
		void render();

	public:
		bool init();
		void release();
		void debugRender();

	public:
		void setParamString( const std::string& _params );

	public:
		const std::string & getPathEntities() const;
		const std::string & getPathScenes() const;
		const std::string & getPathArrows() const;

	public:
		Arrow * getArrow( const std::string & _name );
		void removeArrow( const std::string & _name );

		Arrow * getDefaultArrow();

	public:
		Scene * getScene(const std::string & _name );
		void destroyScene( const std::string & _name );

	public:
		void loader( XmlElement * _xml );
		void loaderGame_( XmlElement * _xml );
		void loaderScenes_( XmlElement * _xml );
		void loaderArrows_( XmlElement * _xml );
		void loaderEntities_( XmlElement * _xml );
		void loaderResources_( XmlElement * _xml );
		void loaderDefault_( XmlElement * _xml );
		void loaderPersonality_( XmlElement * _xml );
		

	public:
		bool handleKeyEvent( size_t _key, bool _isDown ) override;
		bool handleMouseButtonEvent( size_t _button, bool _isDown ) override;
		bool handleMouseMove( int _x, int _y, int _whell ) override;
		void handleMouseLeave();
		void handleMouseEnter();

		void onResourceLoaded() override;
		void onResourceUnLoaded() override;
		
	public:
		const mt::vec2f & getResourceResolution() const;

	protected:

		mt::vec2f	m_resourceResolution;

		std::string m_defaultArrowName;

		std::string m_personality;

		std::string m_eventInit;
		std::string m_eventUpdate;
		std::string m_eventFini;

		PyObject * m_pyPersonality;
		Arrow * m_defaultArrow;

		typedef std::map<std::string, Arrow*> TMapArrow;
		TMapArrow m_mapArrow;

		typedef std::map<std::string, Scene*> TMapScene;
		TMapScene m_mapScene;

		typedef std::list<std::string> TListDeclaration;
		TListDeclaration m_listEntitiesDeclaration;
		TListDeclaration m_listArrowsDeclaration;
		TListDeclaration m_listScenesDeclaration;
		TListDeclaration m_listResourceDeclaration;
		
		std::string m_pathResource;
		std::string m_pathScripts;
		std::string m_pathEntities;
		std::string m_pathScenes;
		std::string m_pathArrows;
	};	
}
