#	pragma once

#	include "Holder.h"

#	include <list>
#	include <map>

class TiXmlElement;

namespace Menge
{
	class Player;

	class Scene;
	class Arrow;

	class Amplifier;
	class DialogManager;

	class Game
	{
	public:
		Game();
		~Game();

	public:
		void update( float _timing );
		void render();
		void loader(TiXmlElement *_xml);

	public:
		bool init();
		void release();
		void debugRender();

	public:
		void addArrow( Arrow *_arrow );
		void removeArrow( const std::string &_name );
		Arrow * getArrow( const std::string &_name );
		Arrow * getDefaultArrow();

	public:
		void addScene( Scene * _scene );
		Scene * getScene(const std::string & _name );

	public:
		void test( const char * _text );

	protected:
		std::string m_defaultArrowName;

		std::string m_personality;

		std::string m_eventInit;
		std::string m_eventUpdate;
		std::string m_eventFini;

		Player * m_player;

		Arrow * m_defaultArrow;
		Amplifier * m_amplifier;
		DialogManager * m_dialogManager;

		typedef std::map<std::string, Arrow*> TMapArrow;
		TMapArrow m_mapArrow;

		typedef std::map<std::string, Scene*> TMapScene;
		TMapScene m_mapScene;

		typedef std::list<std::string> TListEntitysDeclaration;
		TListEntitysDeclaration m_listEntitiesDeclaration;

		std::string m_pathEntities;
	};	
}
