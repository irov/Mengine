#	pragma once

#	include "Core/Holder.h"
#	include "MengeExport.h"
#	include "InputHandler.h"
#	include "Eventable.h"
#	include "Loadable.h"

#	include "Account.h"

#	include "Core/Resolution.h"

#	include <map>

class BinParser;

namespace Menge
{
	class ResourceManager;
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
		, public Loadable
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
		
		void loadPakFromName( const ConstString& _pakName );
		void loadConfigPaks();
		void setLanguagePack( const ConstString& _packName );
		const String & getPakPath( const ConstString& _packName ) const;
		const String& getScreensaverName() const;

	public:
		Arrow * getDefaultArrow();

	public:
		PyObject * getPersonality();

	public:
		//bool loader( const String& _iniFile );
		void loader( XmlElement* _xml ) override;
		void parser( BinParser * _parser ) override;

	protected:
		void _loaded() override;

	public:
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
		const String & getTitle() const;
		int getBits() const;
		bool getFullscreen() const;
		bool getTextureFiltering() const;
		int getFSAAType() const;
		int getFSAAQuality() const;
		bool getHasWindowPanel() const;

	public:
		void addHomeless( Node * _homeless );

	private:
		Node * m_homeless;

		Player* m_player;
		Amplifier* m_amplifier;
		LightSystem* m_lightSystem;

		ConstString m_title;
		bool m_localizedTitle;

		Resolution m_contentResolution;
		Resolution m_resolution;

		bool m_fixedContentResolution;

		ConstString m_defaultArrowName;
		ConstString m_personality;

		String m_screensaverName;

		PyObject * m_pyPersonality;

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

		String m_baseDir;

		typedef std::vector<ResourcePak *> TVectorResourcePak;
		TVectorResourcePak m_paks;
		TVectorResourcePak m_languagePaks;

		ConstString m_languagePak;

		bool m_personalityHasOnClose;

	private:
		void initPredefinedResources_();
		void removePredefinedResources_();
	};	
}
