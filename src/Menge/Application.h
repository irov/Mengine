#	pragma once

#	include "Interface/ApplicationInterface.h"

#   include "Interface/OptionsInterface.h"
#   include "Interface/PrototypeManagerInterface.h"
#	include "Interface/TextInterface.h"
#   include "Interface/InputSystemInterface.h"
#   include "Interface/RenderSystemInterface.h"
#	include "Interface/SoundSystemInterface.h"
#   include "Interface/ParticleSystemInterface.h"
#   include "Interface/ScriptSystemInterface.h"
#   include "Interface/EventInterface.h"
#   include "Interface/PhysicSystemInterface.h"
#   include "Interface/ConfigInterface.h"
#   include "Interface/GameInterface.h"
#   include "Interface/PlayerInterface.h"
#	include "Interface/PrefetcherInterface.h"
#   include "Interface/StringizeInterface.h"
#	include "Interface/NotificationServiceInterface.h"
#   include "Interface/MousePickerSystemInterface.h"
#	include "Interface/LoaderInterface.h"
#	include "Interface/NodeInterface.h"
#	include "Interface/ThreadSystemInterface.h"
#	include "Interface/CodecInterface.h"
#   include "Interface/ConverterInterface.h"
#   include "Interface/ResourceInterface.h"
#   include "Interface/AccountInterface.h"
#   include "Interface/ProfilerInterface.h"
#   include "Interface/GameInterface.h"
#   include "Interface/WatchdogInterface.h"
#   include "Interface/GraveyardInterface.h"
#   include "Interface/PackageInterface.h"
#	include "Interface/UserdataInterface.h"

#   include "Consts.h"

#	include "Core/Resolution.h"
#	include "Core/Viewport.h"
#	include "Core/ConstString.h"

#	include "Logger/Logger.h"

#	include "Math/vec4.h"

#	include <stdex/stl_map.h>

namespace Menge
{
	class ResourceCursor;

	class Application 
		: public ServiceBase<ApplicationInterface>
	{
	public:
		Application();
		~Application();

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		void finalizeGame() override;

	public:
		void parseArguments( const String & _args );

	public:
		bool getAllowFullscreenSwitchShortcut() const override;

	public:
		bool initializeGame( const ConstString & _category, const FilePath & _resourceIniPath ) override;

	public:
		void changeWindowResolution( const Resolution & _resolution ) override;

	public:
		void setFullscreenMode( bool _fullscreen ) override;
		bool getFullscreenMode() const override;
		
	public:
		bool isFocus() const override;

	protected:
		bool registerBaseNodeTypes_();
		bool registerBaseResourceTypes_();
		bool registerSceneGenerator_();

	public:
		bool createRenderWindow() override;

		//void screenshot( const RenderTextureInterfacePtr & _renderTargetImage, const mt::vec4f & _rect );

		void quit();

	public:
		void calcWindowResolution( Resolution & _windowResolution ) const override;

		const Resolution & getCurrentResolution() const override;

		const Viewport & getRenderViewport() const override;
		const Resolution & getContentResolution() const override;

		void getGameViewport( float & _aspect, Viewport & _viewport ) const override;

	public:
		bool render() override;
		void flush() override;
		bool beginUpdate() override;
		void tick( float _timing ) override;
		void endUpdate() override;
		void setFocus( bool _focus, bool _mouse, const mt::vec2f & _point ) override;
		void close() override;

		void turnSound( bool _turn ) override;

	public:
		bool keyEvent( const InputKeyEvent & _event ) override;

		bool mouseButtonEvent( const InputMouseButtonEvent& _event ) override;
		bool mouseMove( const InputMouseMoveEvent& _event ) override;
		bool mouseWheel( const InputMouseWheelEvent & _event ) override;
		void mousePosition( const InputMousePositionEvent & _event ) override;
		void mouseEnter( const InputMousePositionEvent & _event ) override;
		void mouseLeave( const InputMousePositionEvent & _event ) override;

	public:
		void paint() override;

		bool userEvent( const ConstString & _event, const TMapParams & _params ) override;

	public:
		void setParticleEnable( bool _enabled ) override;
		bool getParticleEnable() const override;

		void setTextEnable( bool _enable ) override;
		bool getTextEnable() const override;

	public:
		void setInputMouseButtonEventBlock( bool _block ) override;
		bool getInputMouseButtonEventBlock() const override;

	public:
		void minimizeWindow() override;

		unsigned int getDebugMask() const override;

	public:
		const WString & getCompanyName() const override;
		const WString & getProjectName() const override;

	public:
		const ConstString & getProjectTitle() const override;
		const ConstString & getProjectCodename() const override;
		uint32_t getProjectVersion() const override;

	public:
		const ConstString & getLocale() const override;

	public:
		const Resolution & getWindowResolution() const;

	public:
		void setFixedContentResolution( bool _fixedContetResolution ) override; 
		bool getFixedContentResolution() const override;

	public:
		bool isValidWindowMode() const override;		

	public:
		void enableDebug( bool _enable );

		void updateNotification();
		void setVSync( bool _vsync ) override;
		bool getVSync() const override;
		void setCursorMode( bool _mode ) override;
		bool getCursorMode() const override;
		void setCursorIcon( const ConstString & _resourceName ) override;

		//void setAsScreensaver( bool _set );

		void showKeyboard() override;
		void hideKeyboard() override;

	public:
		void debugPause( bool _pause ) override;
		
	protected:
		bool findBestAspectViewport_( float _aspect, float & _bestAspect, Viewport & _viewport ) const; 

	protected:		
		void calcRenderViewport_( const Resolution & _resolution, Viewport & _viewport );
		void invalidateWindow_();

	protected:
		void notifyDebugOpenFile_( const char * _folder, const char * _fileName );

	protected:
		Resolution m_currentResolution;

		Viewport m_renderViewport;

		Viewport m_gameViewport;

		bool m_particleEnable;
		bool m_textEnable;

		bool m_focus;
		bool m_update;

		Resolution m_windowResolution;
		uint32_t m_bits;
		bool m_fullscreen;
		bool m_vsync;

		Resolution m_renderResolution;

		bool m_textureFiltering;
		int	m_FSAAType;
		int m_FSAAQuality;

		Resolution m_contentResolution;

		typedef stdex::map<float, Viewport> TMapAspectRatioViewports;
		TMapAspectRatioViewports m_aspectRatioViewports;

		bool m_fixedContentResolution;
		bool m_fixedDisplayResolution;
		bool m_createRenderWindow;

		unsigned int m_debugMask;

		bool m_resetTiming;
		float m_phycisTiming;
		float m_maxTiming;

		void parseArguments_( const String& _arguments );

		ResourceCursor * m_cursorResource;

		WString m_companyName;
		WString m_projectName;

		ConstString m_locale;

		ConstString m_projectCodename;
		uint32_t m_projectVersion;

		bool m_invalidateVsync;
		bool m_cursorMode;
		bool m_invalidateCursorMode;
		bool m_mouseEnter;

		bool m_inputMouseButtonEventBlock;

		bool m_resourceCheck;
		bool m_resourceCheckCritical;

		bool m_debugPause;

		bool m_debugFileOpen;
		Observer * m_notifyDebugOpenFile;
	};
}
