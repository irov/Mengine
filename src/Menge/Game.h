#	pragma once

#	include "ResourceManager.h"

#	include "InputHandler.h"
#	include "Eventable.h"
#	include "math/vec3.h"
#	include "Account.h"
#	include "TextField.h"

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
		void render( unsigned int _debugMask = 0 );

	public:
		bool init();
		bool loadPersonality();
		void release();

	public:
		std::string getPathEntities( const std::string& _entity ) const;

	public:
		Arrow * getArrow( const std::string & _name );
		bool loadArrow( const std::string & _name, const std::string & _path );
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

		void readResourceFile( const std::string& _file );
		void loaderResourceFile( XmlElement * _xml );
		void loaderResourceFile_( XmlElement * _xml );

		void loadAccounts();
		void loaderAccounts_( XmlElement* _xml );
		
	public:
		bool handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, int _whell ) override;
		void handleMouseLeave();
		void handleMouseEnter();

		void onResourceLoaded( const std::string& _name ) override;
		void onResourceUnLoaded() override;
		
	public:
		const std::size_t * getResolution() const;
		const std::size_t * getResourceResolution() const;
		bool isContentResolutionFixed() const;

		const std::string& getTitle() const;

		int getBits() const;
		bool getFullscreen() const;
		bool getVSync() const;
		const String& getPhysicSystemName() const;
		bool getTextureFiltering() const;
		int getFSAAType() const;
		int getFSAAQuality() const;

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
		std::string m_title;

		std::size_t	m_resourceResolution[2];
		std::size_t m_resolution[2];

		bool m_fixedContentResolution;

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

		typedef std::pair<std::string, std::string> TPairDeclaration;
		typedef std::list< TPairDeclaration > TListDeclaration;
		typedef std::map<std::string, TPairDeclaration > TMapDeclaration;
		TMapDeclaration m_mapEntitiesDeclaration;
		TMapDeclaration m_mapArrowsDeclaration;
		TMapDeclaration m_mapScenesDeclaration;
		TMapDeclaration m_mapResourceDeclaration;
		TListDeclaration m_listResourceLocation;
		
		std::string m_currentResourcePath;
		TListDeclaration m_pathResource;
		TListDeclaration m_pathScripts;
		TListDeclaration m_pathEntities;
		TListDeclaration m_pathScenes;
		TListDeclaration m_pathArrows;

		int m_bits;
		bool m_fullScreen;
		bool m_vsync;
		bool m_textureFiltering;
		int	m_FSAAType;
		int m_FSAAQuality;
		std::string m_physicSystemName;

		TStringVector m_loadedAccounts;
		typedef std::map<String, Account*> TAccountMap;
		TAccountMap m_accounts;

		bool m_loadingAccounts;
		String m_defaultAccountName;
		Account* m_currentAccount;


		/////
		std::string m_debugResourceFont;
		char m_debugText[128];
		float m_FPS;
	};	
}
