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

	class Amplifier;
	class LightSystem;

	class Game
		: public Holder<Game>
		, public InputHandler
		, public Eventable
		, public Scriptable
		, public Loadable
	{
	public:
		Game( const String & _baseDir, const String & _platformName );
		~Game();

	public:
		bool update();
		void tick( float _timing );
		bool render( unsigned int _debugMask = 0 );

	public:
		bool initialize( const String& _scriptInitParams );
		void finalize();

		void initializeRenderResources();
		void finalizeRenderResources();

		bool loadDescription( const ConstString & _gamePackName, const String & _gameDescription );

		bool loadPersonality();
		
		void loadConfigPaks();
		void applyConfigPaks();
		void setLanguagePack( const ConstString& _packName );
		const String& getScreensaverName() const;

	protected:
		bool loadLocalePaksByName_( const ConstString & _locale, const String & _platform );
			
	public:
		//bool loader( const String& _iniFile );
		void loader( BinParser * _parser ) override;

	protected:
		void _loaded() override;

	protected:
		PyObject * _embedded() override;
		void _embedding( PyObject * _embed ) override;

	public:
		void setCursorMode( bool _mode );

	public:
		bool handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;		
		bool handleMouseButtonEventBegin( const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( const mt::vec2f & _point, float _x, float _y, int _whell ) override;
		
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
		const Resolution & getResolution() const;
		const Resolution & getContentResolution() const;
		bool isContentResolutionFixed() const;
		const String & getTitle() const;
		int getBits() const;
		bool getFullscreen() const;
		bool getTextureFiltering() const;
		int getFSAAType() const;
		int getFSAAQuality() const;
		bool getHasWindowPanel() const;

	public:
		void addHomeless( Node * _homeless );
		void clearHomeless();
		float getTimingFactor() const;
		void setTimingFactor( float _timingFactor );  

	private:
		Node * m_homeless;

		Player* m_player;
		Amplifier* m_amplifier;

		AccountManagerListener * m_accountLister; 
		AccountManager * m_accountManager;
		
		//LightSystem* m_lightSystem;

		ConstString m_title;
		bool m_localizedTitle;

		Resolution m_contentResolution;
		Resolution m_resolution;

		bool m_fixedContentResolution;

		ConstString m_defaultArrowName;
		ConstString m_defaultArrowPrototype;
		ConstString m_personalityModule;

		String m_screensaverName;

		Arrow * m_defaultArrow;

		ConstString m_currentPakName;
		String m_currentResourcePath;

		int m_bits;
		bool m_fullScreen;
		bool m_vsync;
		bool m_textureFiltering;
		int	m_FSAAType;
		int m_FSAAQuality;
		bool m_hasWindowPanel;
		float m_timingFactor;

		String m_baseDir;
		String m_iconPath;

		typedef std::vector<ResourcePak *> TVectorResourcePak;
		TVectorResourcePak m_paks;
		TVectorResourcePak m_resourcePaks;
		TVectorResourcePak m_languagePaks;

		ConstString m_languagePak;
		String m_platformName;

		ConstString m_projectName;
		bool m_personalityHasOnClose;

		typedef std::vector<PyObject *> TVectorHandlers;
		TVectorHandlers m_handlersMouseMove;
		TVectorHandlers m_handlersMouseButton;

	protected:
		void initPredefinedResources_();
		void removePredefinedResources_();
	};	
}
