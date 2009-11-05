#	pragma once

#	include "Holder.h"
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
		void update( float _timing );
		void render( unsigned int _debugMask = 0 );

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
		//std::pair< String, String > getPathEntity( const String& _name ) const;
		//std::pair< String, String > getPathScene( const String& _name ) const;
		//std::pair< String, String > getPathArrow( const String& _name ) const;
		//std::pair< String, String > getPathResource( const String& _name ) const;
		
	public:
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
		Amplifier* m_amplifier;
		LightSystem* m_lightSystem;

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

		typedef std::map<String, std::pair< String, String > > TMapDeclaration;
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
			String path;
			String description;
			bool preload;
		};

		typedef std::vector<ResourcePak> TResourcePakVector;
		TResourcePakVector m_paks;
		ResourcePak m_languagePack;

		//String m_languagePack;
		bool m_personalityHasOnClose;

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
