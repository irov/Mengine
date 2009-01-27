#	pragma once

#	include "InputHandler.h"
#	include "Eventable.h"
#	include "Account.h"
#	include "TextField.h"

#	include "Resolution.h"

#	include <list>
#	include <map>

class XmlElement;

namespace Menge
{
	class ResourceManager;
	class TextManager;
	class Player;

	class Scene;
	class Arrow;

	class Amplifier;

	class Game
		: public InputHandler
		, public Eventable
	{
	public:
		Game();
		~Game();

	public:
		void update( float _timing );
		void render( unsigned int _debugMask = 0 );

	public:
		bool init();
		bool loadPersonality();
		void release();

	public:
		Arrow * getArrow( const String& _name );
		Arrow * getDefaultArrow();

		bool loadArrow( const String& _name );
		void removeArrow( const String& _name );

	public:
		Scene * getScene( const String& _name );
		void destroyScene( const String& _name );

	public:
		bool loader( const String& _iniFile );
		
		void loaderScenes_( XmlElement * _xml );
		void loaderArrows_( XmlElement * _xml );
		void loaderEntities_( XmlElement * _xml );
		void loaderResources_( XmlElement * _xml );

		void readResourceFile( const String& _file );
		void loaderResourceFile( XmlElement * _xml );
		void loaderResourceFile_( XmlElement * _xml );

		void loadAccounts();
		void registerResources( const String & _baseDir );

	public:
		bool handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, int _whell ) override;
		void handleMouseLeave();
		void handleMouseEnter();
		void onFocus( bool _focus );

	public:
		const Resolution & getResolution() const;
		const Resolution & getResourceResolution() const;
		bool isContentResolutionFixed() const;
		String getTitle() const;
		int getBits() const;
		bool getFullscreen() const;
		bool getVSync() const;
		const String& getPhysicSystemName() const;
		bool getTextureFiltering() const;
		int getFSAAType() const;
		int getFSAAQuality() const;
		bool getHasWindowPanel() const;

	public:
		void createAccount( const String& _accountName );
		void deleteAccount( const String& _accountName );
		void selectAccount( const String& _accountName );
		void saveAccount( const String& _accountName );
		void saveAccounts();
		void saveAccountsInfo();
		Account * getCurrentAccount();

	public:
		String getPathEntity( const String& _name ) const;
		String getPathScene( const String& _name ) const;
		String getPathArrow( const String& _name ) const;
		String getPathResource( const String& _name ) const;

		String getCategoryResource( const String& _path ) const;

	public:
		const String& getBaseDirectory() const;			// ../
		const String& getPathGameFile() const;			// Game/Game.ini
		const TStringVector& getResourcePaths() const;	// Game/Resource.xml, Framework/Resource.xml

		const TStringVector& getResourcesNames() const;	// default, level0, level1
		const TStringVector& getEntitiesNames() const;	// PhysicObject, Pepelaz
		const TStringVector& getArrowNames() const;		// default
		const TStringVector& getScenesNames() const;	// Level0, Level17


		// need ?
		const TStringVector& getScriptsPaths() const;	// Game/Scripts, Framework/Scripts
		const TStringVector& getEntitiesPaths() const;	// Game/Entities, Framework/Entities
		const TStringVector& getArrowPaths() const;		// Game/Arrow, Framework/Arrow
		const TStringVector& getScenesPaths() const;	// Game/Scenes, Framework/Scenes
		const TStringVector& getResourcesPaths() const;	// Game/Resource, Framework/Resource
	
	private:

		Player* m_player;
		String m_title;

		Resolution m_resourceResolution;
		Resolution m_resolution;

		bool m_fixedContentResolution;

		String m_defaultArrowName;
		String m_personality;
		String m_eventInit;
		String m_eventUpdate;
		String m_eventFini;

		PyObject * m_pyPersonality;
		Arrow * m_defaultArrow;

		typedef std::map<String, Arrow*> TMapArrow;
		TMapArrow m_mapArrow;

		typedef std::map<String, Scene*> TMapScene;
		TMapScene m_mapScene;

		// data about Game structure:
		String m_baseDir;
		String m_gameFile;

		typedef std::map<String, String> TMapDeclaration;
		TMapDeclaration m_mapEntitiesDeclaration;
		TMapDeclaration m_mapArrowsDeclaration;
		TMapDeclaration m_mapScenesDeclaration;
		TMapDeclaration m_mapResourceDeclaration;
		
		TStringVector m_pathScripts;
		TStringVector m_pathEntities;
		TStringVector m_pathScenes;
		TStringVector m_pathArrows;
		TStringVector m_pathText;

		TStringVector m_pathResource;
		TStringVector m_nameScenes;
		TStringVector m_nameArrows;
		TStringVector m_nameEntities;

		TStringVector m_nameResources;

		TStringVector m_resourcePaths;

		String m_currentResourcePath;

		int m_bits;
		bool m_fullScreen;
		bool m_vsync;
		bool m_textureFiltering;
		int	m_FSAAType;
		int m_FSAAQuality;
		bool m_hasWindowPanel;
		String m_physicSystemName;		

		TStringVector m_accountNames;
		typedef std::map<String, Account*> TAccountMap;
		TAccountMap m_accounts;

		bool m_loadingAccounts;
		String m_defaultAccountName;
		Account* m_currentAccount;

		float m_FPS;

		bool loaderAccounts_( const String& _iniFile );

	private:
		void _initPredefinedResources();
		void _removePredefinedResources();
	};	
}
