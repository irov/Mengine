#	pragma once

#	include <list>
#	include <map>

#	include "InputHandler.h"

class TiXmlElement;

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

namespace Menge
{
	class Player;

	class Scene;
	class Arrow;

	class Amplifier;

	class Game
		: public InputHandler
	{
	public:
		Game();
		~Game();

	public:
		void update( size_t _timing );
		void render();
		void loader(TiXmlElement * _xml);

	public:
		bool init();
		void release();
		void debugRender();

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

	public:
		bool handleKeyEvent( size_t _key, bool _isDown ) override;
		bool handleMouseButtonEvent( size_t _button, bool _isDown ) override;
		bool handleMouseMove( int _x, int _y, int _whell ) override;

	protected:
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
