#	pragma once

#	include "Core/Holder.h"
#	include "MengeExport.h"
#	include "InputHandler.h"
#	include "Scriptable.h"
#	include "Eventable.h"
#	include "Loadable.h"

#	include "Account.h"

#	include "Core/Resolution.h"

#	include <map>

namespace Menge
{
	class ResourceManager;

	class AccountManagerListener;
	class AccountManager;

	class TextManager;
	class Player;
	class ResourcePak;

	class Node;
	class Scene;
	class Arrow;

	class AmplifierServiceInterface;
	class LightSystem;

	class Game
		: public Holder<Game>
		, public InputHandler
		, public Eventable
		, public Scriptable
		, public Loadable
	{
	public:
		Game( const WString & _baseDir, bool _developmentMode, const String & _platformName );
		~Game();

	public:
		bool update();
		void tick( float _timing );
		bool render( unsigned int _debugMask = 0 );

	public:
		bool initialize( const String& _scriptInitParams, const TMapWString & _params );
		void finalize();

		void initializeRenderResources();
		void finalizeRenderResources();

		bool loadPersonality( const ConstString & _module );
		
		bool loadConfigPaks(  const WString & _resourcePackPath );
		void applyConfigPaks();
		void setLanguagePack( const ConstString& _packName );
		const WString& getScreensaverName() const;

	protected:
		bool loadLocalePaksByName_( const ConstString & _locale, const String & _platform );
			
	public:
		//bool loader( const String& _iniFile );
		void loader( BinParser * _parser ) override;

	protected:
		PyObject * _embedded() override;
		void _embedding( PyObject * _embed ) override;

	public:
		void setCursorMode( bool _mode );

	public:
		bool handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;		
		bool handleMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y, int _whell ) override;
		
		void onAppMouseLeave();
		void onAppMouseEnter( const mt::vec2f & _point );

		void onFocus( bool _focus );
		void onFullscreen( const Resolution & _resolution, bool _fullscreen );
		bool onClose();

		void onTurnSound( bool _turn );

	public:
		void addMouseMoveHandler( PyObject * _cb );
		bool removeMouseMoveHandler( PyObject * _cb );

		void addMouseButtonHandler( PyObject * _cb );
		bool removeMouseButtonHandler( PyObject * _cb );

	public:
		const WString & getParam( const WString & _paramName );
		bool hasParam( const WString & _paramName ) const;

		bool getHasWindowPanel() const;

	public:
		void addHomeless( Node * _homeless );
		void clearHomeless();
		float getTimingFactor() const;
		void setTimingFactor( float _timingFactor ); 

	public:
		void destroyArrow();

	private:
		Node * m_homeless;

		Player* m_player;
		AmplifierServiceInterface * m_amplifierService;

		AccountManagerListener * m_accountLister; 
		AccountManager * m_accountManager;

		bool m_developmentMode;

		WString m_screensaverName;

		Arrow * m_defaultArrow;
		
		ConstString m_currentPakName;
		String m_currentResourcePath;

		bool m_hasWindowPanel;
		float m_timingFactor;

		WString m_baseDir;
		WString m_iconPath;

		typedef std::vector<ResourcePak *> TVectorResourcePak;
		TVectorResourcePak m_paks;
		TVectorResourcePak m_resourcePaks;
		TVectorResourcePak m_languagePaks;

		ConstString m_languagePak;
		String m_platformName;
				
		bool m_personalityHasOnClose;

		typedef std::vector<PyObject *> TVectorHandlers;
		TVectorHandlers m_handlersMouseMove;
		TVectorHandlers m_handlersMouseButton;
		TMapWString m_params;

	protected:
		void initPredefinedResources_();
		void removePredefinedResources_();
	};	
}
