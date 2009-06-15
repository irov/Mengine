#	pragma once
#	include "MengeExport.h"
#	include "InputHandler.h"
#	include "Eventable.h"
#	include "Account.h"

#	include "Resolution.h"

#	include <map>

class XmlElement;

namespace Menge
{
	class ResourceManager;
	class TextManager;
	class Player;

	class Node;
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
		bool init( const String& _scriptInitParams );
		bool loadPersonality();
		void release();
		void setBaseDir( const String& _baseDir );
		void loadPak( const String& _pakName );
		void loadConfigPaks();
		void setLanguagePack( const String& _packName );

	public:
		Arrow * getArrow( const String& _name );
		Arrow * getDefaultArrow();

		bool loadArrow( const String& _name );
		void removeArrow( const String& _name );

	public:
		PyObject * getPersonality();

	public:
		Scene * getScene( const String& _name );
		bool destroyScene( Scene * _scene );

	public:
		//bool loader( const String& _iniFile );
		void loader( XmlElement* _xml );
		
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
		void onFullscreen( bool _fullscreen );
		bool onClose();

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
		const TStringVector& getResourcePaths() const;	// Game/Resource.xml, Framework/Resource.xml
		const TStringVector& getResourceFilePaths() const;	// Game/Resource/default.resource

		const TStringVector& getScriptsPaths() const;	// Game/Scripts, Framework/Scripts
		const TStringVector& getEntitiesPaths() const;	// Game/Entities, Framework/Entities
		const TStringVector& getArrowPaths() const;		// Game/Arrow, Framework/Arrow
		const TStringVector& getScenesPaths() const;	// Game/Scenes, Framework/Scenes
		const TStringVector& getResourcesPaths() const;	// Game/Resource, Framework/Resource
		const TStringVector& getTextsPaths() const;


	public:
		void addHomeless( PyObject * _homeless );

	private:
		typedef std::list<PyObject *> TContainerHomeless;
		TContainerHomeless m_homeless;

		Player* m_player;
		String m_title;
		bool m_localizedTitle;

		Resolution m_resourceResolution;
		Resolution m_resolution;

		bool m_fixedContentResolution;

		String m_defaultArrowName;
		String m_personality;

		PyObject * m_pyPersonality;
		Arrow * m_defaultArrow;

		typedef std::map<String, Arrow*> TMapArrow;
		TMapArrow m_mapArrow;

		typedef std::map<String, Scene*> TMapScene;
		TMapScene m_mapScene;

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

		TStringVector m_pathResourceFiles;

		TStringVector m_pathResource;

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

		bool loaderAccounts_( const String& _iniFile );
		String m_baseDir;

		struct ResourcePak
		{
			String name;
			String description;
			bool preload;
		};

		String m_languagePack;

	private:
		void initPredefinedResources_();
		void removePredefinedResources_();
	};	
}
