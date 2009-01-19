#	pragma once

//#	include "ResourceManager.h"

#	include "InputHandler.h"
#	include "Eventable.h"
#	include "math/vec3.h"
#	include "Account.h"
#	include "TextField.h"

#	include "Resolution.h"

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
		String getPathEntities( const String& _entity ) const;

	public:
		Arrow * getArrow( const String& _name );
		bool loadArrow( const String& _name, const String& _path );
		void removeArrow( const String& _name );

		Arrow * getDefaultArrow();

	public:
		Scene * getScene( const String& _name );
		void destroyScene( const String& _name );

	public:
		bool loader( const String& _iniFile );
		//void loaderGame_( XmlElement * _xml );
		void loaderScenes_( XmlElement * _xml );
		void loaderArrows_( XmlElement * _xml );
		void loaderEntities_( XmlElement * _xml );
		void loaderResources_( XmlElement * _xml );

		void readResourceFile( const String& _file );
		void loaderResourceFile( XmlElement * _xml );
		void loaderResourceFile_( XmlElement * _xml );

		void loadAccounts();
		void registerResources( const String& _baseDir );

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

		void createAccount( const String& _accountName );
		void deleteAccount( const String& _accountName );
		void selectAccount( const String& _accountName );
		void saveAccount( const String& _accountName );
		void saveAccounts();

		Account * getCurrentAccount();

		void saveAccountsInfo();

	protected:

		TextField * m_debugTextField;
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

		typedef std::pair<String, String> TPairDeclaration;
		typedef std::list< TPairDeclaration > TListDeclaration;
		typedef std::map<String, TPairDeclaration > TMapDeclaration;
		TMapDeclaration m_mapEntitiesDeclaration;
		TMapDeclaration m_mapArrowsDeclaration;
		TMapDeclaration m_mapScenesDeclaration;
		TMapDeclaration m_mapResourceDeclaration;
		TListDeclaration m_listResourceLocation;
		
		String m_currentResourcePath;
		TListDeclaration m_pathResource;
		TListDeclaration m_pathScripts;
		TListDeclaration m_pathEntities;
		TListDeclaration m_pathScenes;
		TListDeclaration m_pathArrows;
		TListDeclaration m_pathText;

		TStringVector m_resourcePaths;

		int m_bits;
		bool m_fullScreen;
		bool m_vsync;
		bool m_textureFiltering;
		int	m_FSAAType;
		int m_FSAAQuality;
		String m_physicSystemName;
		bool m_hasWindowPanel;

		TStringVector m_accountNames;
		typedef std::map<String, Account*> TAccountMap;
		TAccountMap m_accounts;

		bool m_loadingAccounts;
		String m_defaultAccountName;
		Account* m_currentAccount;


		/////
		String m_debugResourceFont;
		char m_debugText[128];
		float m_FPS;

		bool loaderAccounts_( const String& _iniFile );

	private:
		void _initPredefinedResources();
		void _removePredefinedResources();
	};	
}
