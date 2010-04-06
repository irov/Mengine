#	pragma once

#	include "Core/Holder.h"
#	include "MengeExport.h"
#	include "InputHandler.h"
#	include "Eventable.h"
#	include "Account.h"

#	include "Core/Resolution.h"

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
	class LightSystem;

	class Game
		: public InputHandler
		, public Eventable
		, public Holder<Game>
	{
	public:
		Game();
		~Game();

	public:
		void update();
		void tick( float _timing );
		bool render( unsigned int _debugMask = 0 );

	public:
		bool init( const String& _scriptInitParams );
		bool loadPersonality();
		void release();
		void setBaseDir( const String& _baseDir );
		void loadPak( const String& _pakName, const String& _pakPath, const String& _descFilename );
		void loadPakFromName( const String& _pakName );
		void loadConfigPaks();
		void setLanguagePack( const String& _packName );
		String getPakPath( const String& _packName );

	public:
		Arrow * getArrow( const String& _name );
		Arrow * getDefaultArrow();

		bool loadArrow( const String& _pakName, const String& _name );
		void removeArrow( const String& _name );

	public:
		PyObject * getPersonality();

	public:
		Scene * getScene( const String& _name );
		bool destroyScene( Scene * _scene );
		bool destroySceneByName( const String & _sceneName );

	public:
		//bool loader( const String& _iniFile );
		void loader( XmlElement* _xml );
		
		void loaderScenes_( XmlElement * _xml );
		void loaderArrows_( XmlElement * _xml );
		void loaderEntities_( XmlElement * _xml );
		void loaderResources_( XmlElement * _xml );

		void readResourceFile( const String& _fileSystemName, const String& _path, const String& _descFile );
		void loaderResourceFile( XmlElement * _xml );
		void loaderResourceFile_( XmlElement * _xml );

		void loadAccounts();
		void setCursorMode( bool _mode );

	public:
		bool handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, int _whell ) override;
		void handleMouseLeave();
		void handleMouseEnter();
		void onFocus( bool _focus );
		void onFullscreen( bool _fullscreen );
		bool onClose();

	public:
		const Resolution & getResolution() const;
		const Resolution & getContentResolution() const;
		bool isContentResolutionFixed() const;
		String getTitle() const;
		int getBits() const;
		bool getFullscreen() const;
		const String& getPhysicSystemName() const;
		bool getTextureFiltering() const;
		int getFSAAType() const;
		int getFSAAQuality() const;
		bool getHasWindowPanel() const;

	public:
		String createNewAccount();
		void deleteAccount( const String& _accountID );
		void selectAccount( const String& _accountID );
		void saveAccount( const String& _accountID );
		void saveAccounts();
		void saveAccountsInfo();
		Account * getCurrentAccount();
		Account * getAccount( const String& _accountID );
		
	public:
		const TVectorString& getResourceFilePaths() const;	// Game/Resource/default.resource

		const TVectorString& getScriptsPaths() const;	// Game/Scripts, Framework/Scripts
		const TVectorString& getEntitiesPaths() const;	// Game/Entities, Framework/Entities
		const TVectorString& getArrowPaths() const;		// Game/Arrow, Framework/Arrow
		const TVectorString& getScenesPaths() const;	// Game/Scenes, Framework/Scenes
		const TVectorString& getResourcesPaths() const;	// Game/Resource, Framework/Resource
		const TVectorString& getTextsPaths() const;


	public:
		void addHomeless( Node * _homeless );

	private:
		Node * m_homeless;

		Player* m_player;
		Amplifier* m_amplifier;
		LightSystem* m_lightSystem;

		String m_title;
		bool m_localizedTitle;

		Resolution m_contentResolution;
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

		typedef std::map<String, std::pair< String, String > > TMapDeclaration;
		TMapDeclaration m_mapEntitiesDeclaration;
		TMapDeclaration m_mapArrowsDeclaration;
		TMapDeclaration m_mapScenesDeclaration;
		TMapDeclaration m_mapResourceDeclaration;
		
		TVectorString m_pathScripts;
		TVectorString m_pathEntities;
		TVectorString m_pathScenes;
		TVectorString m_pathArrows;
		TVectorString m_pathText;

		TVectorString m_pathResourceFiles;

		TVectorString m_pathResource;

		//TStringVector m_resourcePaths;

		String m_currentPakName;
		String m_currentResourcePath;

		int m_bits;
		bool m_fullScreen;
		bool m_vsync;
		bool m_textureFiltering;
		int	m_FSAAType;
		int m_FSAAQuality;
		bool m_hasWindowPanel;
		String m_physicSystemName;		

		//TStringVector m_accountIDs;

		//bool m_loadingAccounts;
		typedef std::map< String, Account* > TAccountMap;

		TAccountMap m_accounts;
		String m_defaultAccountID;
		Account* m_currentAccount;

		//bool loaderAccounts_( const String& _iniFile );
		void loaderAccounts_( XmlElement* _xml );
		Account* loadAccount_( const String& _accountID );
		void createAccount_( const String& _accountID );

		String m_baseDir;

		struct ResourcePak
		{
			String name;
			String path;
			String description;
			bool preload;
		};

		typedef std::vector<ResourcePak> TResourcePakVector;
		TResourcePakVector m_paks;
		//ResourcePak m_languagePack;
		TResourcePakVector m_languagePaks;
		String m_languagePack;

		bool m_personalityHasOnClose;
		int m_playerNumberCounter;

	private:
		void initPredefinedResources_();
		void removePredefinedResources_();

		struct PakFinder
		{
			String m_pakName;

			PakFinder( const String& _pakName )
				: m_pakName( _pakName )
			{
			}

			bool operator()( const ResourcePak& _pak )
			{
				return _pak.name == m_pakName;
			}
		};
	};	
}
