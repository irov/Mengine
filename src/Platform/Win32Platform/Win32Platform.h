#	pragma once

#	include "Interface/PlatformInterface.h"

#	include "Win32MouseEvent.h"
#	include "Win32AlreadyRunningMonitor.h"
#	include "Win32FPSMonitor.h"

namespace Menge
{
	class Win32Platform
		: public ServiceBase<PlatformInterface>
	{
	public:
		Win32Platform();
		~Win32Platform();

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		void update() override;
		void stop() override;

	public:
		bool createWindow( uint32_t _icon, const Menge::WString & _projectTitle, const Resolution & _resolution, bool _fullscreen ) override;
		WindowHandle getWindowHandle() const override;

	public:
		const ConstString & getPlatformName() const override;
		bool isTouchpad() const override;

	public:
		void getDesktopResolution( Resolution & _resolution ) const override;

		size_t getCurrentPath( WChar * _path, size_t _len ) const override;

		void minimizeWindow() override;

		void setCursorPosition( const mt::vec2f & _pos ) override;

		void showKeyboard() override;
		void hideKeyboard() override;

		void notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen ) override;
		void notifyVsyncChanged( bool _vsync ) override;
		void notifyCursorModeChanged( bool _mode ) override;
		bool notifyCursorIconSetup( const ConstString & _name, const FilePath & _path, const MemoryInterfacePtr & _buffer ) override;

	public:
		void onEvent( const ConstString & _event, const TMapParams & _params ) override;

	public:
		size_t getShortPathName( const WString & _path, WChar * _short, size_t _len ) const override;

	public:
		void getMaxClientResolution( Resolution & _resolution ) const override;

	public:
		bool openUrlInDefaultBrowser( const WString & _url ) override;

	public:
		bool createDirectoryUserPicture( const WString & _path, const WString & _file, const void * _data, size_t _size ) override;
		bool createDirectoryUserMusic( const WString & _path, const WString & _file, const void * _data, size_t _size ) override;

	public:
		bool concatenateFilePath( const FilePath & _folder, const FilePath & _fileName, WChar * _filePath, size_t _capacity ) override;

	protected:
		bool setProcessDPIAware();

	public:
		LRESULT wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool wndProcInput( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT & _result );

	protected:
		bool calcCursorPosition_( mt::vec2f & _point ) const;
		WChar translateVirtualKey_( unsigned int _vkc, unsigned int _vk ) const;
		DWORD getWindowStyle_( bool _fullsreen ) const;
		RECT getWindowsRect_( const Resolution & _resolution, bool _fullsreen ) const;

	protected:
		void setActive_( bool _active );

	protected:		
		HINSTANCE m_hInstance;

		HWND m_hWnd;
				
		Win32AlreadyRunningMonitor * m_alreadyRunningMonitor;

		Win32FPSMonitor * m_fpsMonitor;

		Win32MouseEvent m_mouseEvent;

		ConstString m_platformName;
		Resolution m_windowResolution;

		bool m_active;
		bool m_update;
		bool m_close;

		float m_pauseUpdatingTime;

		bool m_vsync;

		bool m_cursorInArea;
		bool m_clickOutArea;
		bool m_cursorMode;

		HCURSOR m_cursor;

		typedef stdex::map<ConstString, HCURSOR> TMapCursors;
		TMapCursors m_cursors;

		bool m_lastMouse;
		int m_lastMouseX;
		int m_lastMouseY;

		bool m_isDoubleClick;
		bool m_touchpad;
	};
}
