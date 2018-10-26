#include "PythonWrapper.h"

#include "Interface/ScriptSystemInterface.h"
#include "Interface/GameInterface.h"
#include "Interface/ConfigInterface.h"

#include "Environment/Python/PythonEventReceiver.h"

#include "Kernel/Logger.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class PythonGameEventReceiver
			: public GameEventReceiver
			, public PythonEventReceiver
		{
		protected:
			void onGameFullscreen(bool _fullscreen) override
			{
				m_cb.call(_fullscreen);
			}

			void onGameFixedContentResolution(bool _fixed) override
			{
				m_cb.call(_fixed);
			}

			void onGameFixedDisplayResolution(bool _fixed) override
			{
				m_cb.call(_fixed);
			}

			void onGameRenderViewport(const Viewport & _viewport, const Resolution & _contentResolution) override
			{
				m_cb.call(_viewport, _contentResolution);
			}

			void onGameViewport(const Viewport & _viewport, float _aspect) override
			{
				m_cb.call(_viewport, _aspect);
			}

			bool onGameKey(KeyCode _code, float _x, float _y, bool _isDown, bool _isRepeat) override
			{
				return m_cb.call((uint32_t)_code, _x, _y, _isDown, _isRepeat);
			}

			bool onGameText(WChar _key, float _x, float _y) override
			{
				return m_cb.call(_key, _x, _y);
			}

			bool onGameMouseButton(uint32_t _touchId, float _x, float _y, uint32_t _button, bool _isDown) override
			{
				return m_cb.call(_touchId, _x, _y, _button, _isDown);
			}

			bool onGameMouseButtonBegin(uint32_t _touchId, float _x, float _y, uint32_t _button, bool _isDown) override
			{
				return m_cb.call(_touchId, _x, _y, _button, _isDown);
			}

			bool onGameMouseButtonEnd(uint32_t _touchId, float _x, float _y, uint32_t _button, bool _isDown) override
			{
				return m_cb.call(_touchId, _x, _y, _button, _isDown);
			}

			bool onGameMouseMove(uint32_t _touchId, float _x, float _y, float _dx, float _dy) override
			{
				return m_cb.call(_touchId, _x, _y, _dx, _dy);
			}

			bool onGameMouseWheel(uint32_t _button, float _x, float _y, int32_t _wheel) override
			{
				return m_cb.call(_button, _x, _y, _wheel);
			}

			void onGameAppMouseEnter(float _x, float _y) override
			{
				m_cb.call(_x, _y);
			}

			void onGameAppMouseLeave() override
			{
				m_cb.call();
			}

			void onGameTimingFactor(float _timingFactor) override
			{
				m_cb.call(_timingFactor);
			}

			bool onGamePreparation(bool _debug) override
			{
				return m_cb.call(_debug);
			}

			void onGameRun() override
			{
				m_cb.call();
			}

			bool onGameInitialize() override
			{
				return m_cb.call();
			}

			void onGameInitializeRenderResources() override
			{
				m_cb.call();
			}

			void onGameFinalizeRenderResources() override
			{
				m_cb.call();
			}

			void onGameAccountFinalize() override
			{
				m_cb.call();
			}

			void onGameFinalize() override
			{
				m_cb.call();
			}

			void onGameDestroy() override
			{
				m_cb.call();
			}

			void onGameFocus(bool _focus) override
			{
				m_cb.call(_focus);
			}

			void onGameCreateDefaultAccount() override
			{
				m_cb.call();
			}

			void onGameCreateGlobalAccount() override
			{
				m_cb.call();
			}

			void onGameLoadAccounts() override
			{
				m_cb.call();
			}

			void onGameCreateAccount(const ConstString & _accountID, bool _global) override
			{
				m_cb.call(_accountID, _global);
			}

			void onGameDeleteAccount(const ConstString & _accountID) override
			{
				m_cb.call(_accountID);
			}

			void onGameSelectAccount(const ConstString & _accountID) override
			{
				m_cb.call(_accountID);
			}

			void onGameUselectAccount(const ConstString & _accountID) override
			{
				m_cb.call(_accountID);
			}

			void onGameChangeSoundVolume(float _sound, float _music, float _voice) override
			{
				m_cb.call(_sound, _music, _voice);
			}

			void onGameCursorMode(bool _mode) override
			{
				m_cb.call(_mode);
			}

			void onGameUser(const ConstString & _event, const MapWParams & _params) override
			{
				m_cb.call(_event, _params);
			}

			bool onGameClose() override
			{
				return m_cb.call();
			}

			void onGameOverFillrate(double _fillrate) override
			{
				m_cb.call(_fillrate);
			}

			void onGameFrameEnd() override
			{
				m_cb.call();
			}
		};
	}
	//////////////////////////////////////////////////////////////////////////
	static void registerGameEventMethods( const ScriptModuleInterfacePtr & _module )
	{
		const pybind::module & base_module = _module->getModule();

		pybind::module py_module(base_module);

		GameServiceInterface * game = GAME_SERVICE();

		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onFullscreen", EVENT_GAME_FULLSCREEN);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onFixedContentResolution", EVENT_GAME_FIXED_CONTENT_RESOLUTION);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onFixedDisplayResolution", EVENT_GAME_FIXED_DISPLAY_RESOLUTION);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onRenderViewport", EVENT_GAME_RENDER_VIEWPORT);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onGameViewport", EVENT_GAME_VIEWPORT);

		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onHandleKeyEvent", EVENT_GAME_KEY);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onHandleTextEvent", EVENT_GAME_TEXT);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onHandleMouseButtonEvent", EVENT_GAME_MOUSE_BUTTON);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onHandleMouseButtonEventBegin", EVENT_GAME_MOUSE_BUTTON_BEGIN);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onHandleMouseButtonEventEnd", EVENT_GAME_MOUSE_BUTTON_END);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onHandleMouseMove", EVENT_GAME_MOUSE_MOVE);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onHandleMouseWheel", EVENT_GAME_MOUSE_WHEEL);

		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onAppMouseEnter", EVENT_GAME_APP_MOUSE_ENTER);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onAppMouseLeave", EVENT_GAME_APP_MOUSE_LEAVE);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onTimingFactor", EVENT_GAME_ON_TIMING_FACTOR);

		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onPreparation", EVENT_GAME_PREPARATION);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onRun", EVENT_GAME_RUN);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onInitialize", EVENT_GAME_INITIALIZE);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onInitializeRenderResources", EVENT_GAME_INITIALIZE_RENDER_RESOURCES);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onAccountFinalize", EVENT_GAME_ACCOUNT_FINALIZE);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onFinalize", EVENT_GAME_FINALIZE);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onDestroy", EVENT_GAME_DESTROY);

		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onFocus", EVENT_GAME_FOCUS);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onCreateDefaultAccount", EVENT_GAME_CREATE_DEFAULT_ACCOUNT);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onCreateGlobalAccount", EVENT_GAME_CREATE_GLOBAL_ACCOUNT);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onLoadAccounts", EVENT_GAME_LOAD_ACCOUNTS);

		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onCreateAccount", EVENT_GAME_CREATE_ACCOUNT);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onDeleteAccount", EVENT_GAME_DELETE_ACCOUNT);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onSelectAccount", EVENT_GAME_SELECT_ACCOUNT);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onUnselectAccount", EVENT_GAME_UNSELECT_ACCOUNT);

		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onChangeSoundVolume", EVENT_GAME_CHANGE_SOUND_VOLUME);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onCursorMode", EVENT_GAME_CURSOR_MODE);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onUserEvent", EVENT_GAME_USER);
		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onCloseWindow", EVENT_GAME_CLOSE);

		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onOverFillrate", EVENT_GAME_OVER_FILLRATE);

		Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>(py_module, game, "onFrameEnd", EVENT_GAME_FRAME_END);
	}
	//////////////////////////////////////////////////////////////////////////
	bool PythonWrapper::gameWrap()
	{
		ConstString personality = CONFIG_VALUE("Game", "PersonalityModule", STRINGIZE_FILEPATH_LOCAL("Personality"));

		ScriptModuleInterfacePtr module = SCRIPT_SERVICE()
			->importModule(personality);

		if (module == nullptr)
		{
			LOGGER_ERROR("Game::loadPersonality invalid import module '%s'"
				, personality.c_str()
			);

			return false;
		}

		registerGameEventMethods( module );

		return true;
	}
}