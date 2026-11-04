#include "DevelopmentHotkeyPlugin.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/TimelineServiceInterface.h"
#include "Interface/FactoryServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/TransformationInterface.h"
#include "Interface/AnimationInterface.h"

#include "DevelopmentHotkeyReloadLocaleSceneChange.h"

#include "Kernel/PluginHelper.h"
#include "Kernel/BuildMode.h"
#include "Kernel/Logger.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Scene.h"

#include "Config/StdIntTypes.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( DevelopmentHotkey, Mengine::DevelopmentHotkeyPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        constexpr int32_t TIME_FACTOR_ALT_STEP_MULTIPLIER = 10;
        //////////////////////////////////////////////////////////////////////////
        static void logTimeFactor()
        {
            float timeFactor = TIMELINE_SERVICE()
                ->calcTimeFactor();

            LOGGER_MESSAGE_RELEASE( "time factor: %f"
                , timeFactor
            );
        }
        //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DEVICE_DESKTOP)
        static void printChildren2( const NodePtr & _node, uint32_t _tab )
        {
            _node->foreachChildren( [_tab]( const NodePtr & _child )
            {
                bool enable = _child->isEnable();

                TransformationInterface * transformation = _child->getTransformation();

                LOGGER_MESSAGE_RELEASE( "%.*s-%s %s[%s] |%p| lp (%.2f, %.2f, %.2f) lo (%.2f, %.2f, %.2f) wp (%.2f, %.2f, %.2f) %s"
                    , _tab
                    , "                                         "
                    , (enable == false) ? "[DISABLE] " : ""
                    , _child->getName().c_str()
                    , Helper::getFactorableType( _child ).c_str()
                    , _child.get()
                    , transformation->getLocalPosition().x
                    , transformation->getLocalPosition().y
                    , transformation->getLocalPosition().z
                    , transformation->getLocalOrientation().x
                    , transformation->getLocalOrientation().y
                    , transformation->getLocalOrientation().z
                    , transformation->getWorldPosition().x
                    , transformation->getWorldPosition().y
                    , transformation->getWorldPosition().z
                    , _child->getAnimation() == nullptr ? "" : (_child->getAnimation()->isPlay() == true ? "[Play]" : "[Stop]")
                );

                if( enable == false )
                {
                    return;
                }

                Detail::printChildren2( _child, _tab + 1 );
            } );
        }
        //////////////////////////////////////////////////////////////////////////
        static void printChildren( const NodePtr & _node )
        {
            if( _node == nullptr )
            {
                return;
            }

            Detail::printChildren2( _node, 0 );
        }
#endif
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    DevelopmentHotkeyPlugin::DevelopmentHotkeyPlugin()
        : m_debugFileOpen( false )
        , m_debugResourceCompile( false )
        , m_debugTextMode( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevelopmentHotkeyPlugin::~DevelopmentHotkeyPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevelopmentHotkeyPlugin::_initializePlugin()
    {
        bool OPTION_logopenfiles = HAS_OPTION( "logopenfiles" );

        if( OPTION_logopenfiles == true )
        {
            m_debugFileOpen = true;

            this->updateDebugOpenFile_();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevelopmentHotkeyPlugin::_finalizePlugin()
    {
        if( m_debugFileOpen == true )
        {
            NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_DEBUG_OPEN_FILE );
        }

        if( m_debugResourceCompile == true )
        {
            NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_COMPILE );
            NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_RELEASE );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevelopmentHotkeyPlugin::_startPlugin()
    {
#if defined(MENGINE_DEVICE_DESKTOP)
        bool developmentMode = Helper::isDevelopmentMode();

        if( developmentMode == false )
        {
            return true;
        }

        m_hotkeys.addHotkey( KC_F3, [this]( const InputKeyEvent & _event )
        {
            MENGINE_UNUSED( _event );

            m_debugResourceCompile = !m_debugResourceCompile;

            this->updateDebugResourceCompile_();
        }, MENGINE_DOCUMENT_FACTORABLE );

        m_hotkeys.addHotkey( KC_F4, [this]( const InputKeyEvent & _event )
        {
            MENGINE_UNUSED( _event );

            m_debugFileOpen = !m_debugFileOpen;

            this->updateDebugOpenFile_();
        }, MENGINE_DOCUMENT_FACTORABLE );

        m_hotkeys.addHotkey( KC_F5, []( const InputKeyEvent & _event )
        {
            if( _event.special.isControl == true )
            {
                SCENE_SERVICE()
                    ->removeCurrentScene( false, nullptr );

                return;
            }

            SCENE_SERVICE()
                ->restartCurrentScene( false, nullptr );
        }, MENGINE_DOCUMENT_FACTORABLE );

        m_hotkeys.addHotkey( KC_F7, []( const InputKeyEvent & _event )
        {
            MENGINE_UNUSED( _event );

            const ScenePtr & scene = SCENE_SERVICE()
                ->getCurrentScene();

            Detail::printChildren( scene );
        }, MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_DEBUG_FACTORY_ENABLE)
        m_hotkeys.addHotkey( KC_G, []( const InputKeyEvent & _event )
        {
            if( _event.special.isControl == false )
            {
                return;
            }

            FACTORY_SERVICE()
                ->debugFactoryIncrefGeneration();
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        m_hotkeys.addHotkey( KC_OEM_MINUS, []( const InputKeyEvent & _event )
        {
            MENGINE_UNUSED( _event );

            const RenderPipelineInterfacePtr & renderPipeline = APPLICATION_SERVICE()
                ->getRenderPipeline();

            if( renderPipeline->decrefDebugLimitRenderObjects() == true )
            {
                return;
            }

            APPLICATION_SERVICE()
                ->setDebugPause( false );

            renderPipeline->enableDebugStepRenderMode( false );
        }, MENGINE_DOCUMENT_FACTORABLE );

        m_hotkeys.addHotkey( KC_MULTIPLY, []( const InputKeyEvent & _event )
        {
            MENGINE_UNUSED( _event );

            bool debugPause = APPLICATION_SERVICE()
                ->getDebugPause();

            debugPause = !debugPause;

            APPLICATION_SERVICE()
                ->setDebugPause( debugPause );
        }, MENGINE_DOCUMENT_FACTORABLE );

        m_hotkeys.addHotkey( KC_OEM_PLUS, []( const InputKeyEvent & _event )
        {
            MENGINE_UNUSED( _event );

            const RenderPipelineInterfacePtr & renderPipeline = APPLICATION_SERVICE()
                ->getRenderPipeline();

            renderPipeline->increfDebugLimitRenderObjects();

            bool debugPause = APPLICATION_SERVICE()
                ->getDebugPause();

            if( debugPause == false )
            {
                renderPipeline->enableDebugStepRenderMode( true );
            }

            APPLICATION_SERVICE()
                ->setDebugPause( true );
        }, MENGINE_DOCUMENT_FACTORABLE );

        this->addTimeFactorHotkey_( KC_P, 0.f );
        this->addTimeFactorHotkey_( KC_0, 0.5f );
        this->addTimeFactorHotkey_( KC_1, 1.f );
        this->addTimeFactorHotkey_( KC_2, 2.f );
        this->addTimeFactorHotkey_( KC_3, 3.f );
        this->addTimeFactorHotkey_( KC_4, 4.f );
        this->addTimeFactorHotkey_( KC_5, 5.f );
        this->addTimeFactorHotkey_( KC_6, 6.f );
        this->addTimeFactorHotkey_( KC_7, 7.f );
        this->addTimeFactorHotkey_( KC_8, 8.f );
        this->addTimeFactorHotkey_( KC_9, 9.f );

        this->addTimeFactorCountHotkey_( KC_OEM_6, 1 );
        this->addTimeFactorCountHotkey_( KC_OEM_4, -1 );

        m_hotkeys.addHotkey( KC_P, []( const InputKeyEvent & _event )
        {
            if( _event.special.isControl == false )
            {
                return;
            }

            bool particleEnable = APPLICATION_SERVICE()
                ->getParticleEnable();

            particleEnable = !particleEnable;

            APPLICATION_SERVICE()
                ->setParticleEnable( particleEnable );

            LOGGER_MESSAGE_RELEASE( "particle [%s]"
                , particleEnable == true ? "enable" : "disable"
            );
        }, MENGINE_DOCUMENT_FACTORABLE );

        m_hotkeys.addHotkey( KC_T, []( const InputKeyEvent & _event )
        {
            if( _event.special.isControl == false )
            {
                return;
            }

            bool textEnable = APPLICATION_SERVICE()
                ->getTextEnable();

            textEnable = !textEnable;

            APPLICATION_SERVICE()
                ->setTextEnable( textEnable );

            LOGGER_MESSAGE_RELEASE( "text [%s]"
                , textEnable == true ? "enable" : "disable"
            );
        }, MENGINE_DOCUMENT_FACTORABLE );

        m_hotkeys.addHotkey( KC_V, []( const InputKeyEvent & _event )
        {
            if( _event.special.isControl == false )
            {
                return;
            }

            bool videoEnable = APPLICATION_SERVICE()
                ->getVideoEnable();

            videoEnable = !videoEnable;

            APPLICATION_SERVICE()
                ->setVideoEnable( videoEnable );

            LOGGER_MESSAGE_RELEASE( "video [%s]"
                , videoEnable == true ? "enable" : "disable"
            );
        }, MENGINE_DOCUMENT_FACTORABLE );

        m_hotkeys.addHotkey( KC_R, [this]( const InputKeyEvent & _event )
        {
            if( _event.special.isControl == false )
            {
                return;
            }

            m_debugTextMode = !m_debugTextMode;

            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_TEXT_MODE, m_debugTextMode );
        }, MENGINE_DOCUMENT_FACTORABLE );

        m_hotkeys.addHotkey( KC_E, [this]( const InputKeyEvent & _event )
        {
            if( _event.special.isControl == false )
            {
                return;
            }

            this->reloadLocale_();
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevelopmentHotkeyPlugin::_stopPlugin()
    {
        m_hotkeys.removeHotkeys();
    }
    //////////////////////////////////////////////////////////////////////////
    void DevelopmentHotkeyPlugin::addTimeFactorHotkey_( EKeyCode _code, float _timeFactorBase )
    {
        m_hotkeys.addHotkey( _code, [_timeFactorBase]( const InputKeyEvent & _event )
        {
            if( _event.special.isAlt == false )
            {
                return;
            }

            TIMELINE_SERVICE()
                ->setTimeFactorBase( _timeFactorBase );

            Detail::logTimeFactor();
        }, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevelopmentHotkeyPlugin::addTimeFactorCountHotkey_( EKeyCode _code, int32_t _timeFactorStep )
    {
        m_hotkeys.addHotkey( _code, [_timeFactorStep]( const InputKeyEvent & _event )
        {
            int32_t timeFactorStep = _timeFactorStep;

            if( _event.special.isAlt == true )
            {
                timeFactorStep *= Detail::TIME_FACTOR_ALT_STEP_MULTIPLIER;
            }

            int32_t timeFactorCount = TIMELINE_SERVICE()
                ->getTimeFactorCount();

            timeFactorCount += timeFactorStep;

            TIMELINE_SERVICE()
                ->setTimeFactorCount( timeFactorCount );

            Detail::logTimeFactor();
        }, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevelopmentHotkeyPlugin::updateDebugOpenFile_()
    {
        if( m_debugFileOpen == true )
        {
            NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_DEBUG_OPEN_FILE, &DevelopmentHotkeyPlugin::notifyDebugOpenFile_, MENGINE_DOCUMENT_FACTORABLE );
        }
        else
        {
            NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_DEBUG_OPEN_FILE );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DevelopmentHotkeyPlugin::notifyDebugOpenFile_( const FilePath & _folder, const FilePath & _filePath, bool _read, bool _streaming )
    {
        bool isMainThread = THREAD_SERVICE()
            ->isMainThread();

        if( isMainThread == true )
        {
            if( _streaming == false )
            {
                LOGGER_VERBOSE_LEVEL( "debug", LM_MESSAGE_RELEASE, LFILTER_NONE, LCOLOR_GREEN, nullptr, 0, nullptr, LFLAG_SHORT )("open %s %s%s"
                    , _read == true ? "read" : "write"
                    , _folder.c_str()
                    , _filePath.c_str()
                    );
            }
            else
            {
                LOGGER_VERBOSE_LEVEL( "debug", LM_MESSAGE_RELEASE, LFILTER_NONE, LCOLOR_GREEN, nullptr, 0, nullptr, LFLAG_SHORT )("streaming %s %s%s"
                    , _read == true ? "read" : "write"
                    , _folder.c_str()
                    , _filePath.c_str()
                    );
            }
        }
        else
        {
            ThreadId tid = THREAD_SYSTEM()
                ->getCurrentThreadId();

            if( _streaming == false )
            {
                LOGGER_VERBOSE_LEVEL( "debug", LM_MESSAGE_RELEASE, LFILTER_NONE, LCOLOR_GREEN, nullptr, 0, nullptr, LFLAG_SHORT )("open [multithread:%" MENGINE_PRIu64 "] open %s %s%s"
                    , tid
                    , _read == true ? "read" : "write"
                    , _folder.c_str()
                    , _filePath.c_str()
                    );
            }
            else
            {
                LOGGER_VERBOSE_LEVEL( "debug", LM_MESSAGE_RELEASE, LFILTER_NONE, LCOLOR_GREEN, nullptr, 0, nullptr, LFLAG_SHORT )("streaming [multithread:%" MENGINE_PRIu64 "] open %s %s%s"
                    , tid
                    , _read == true ? "read" : "write"
                    , _folder.c_str()
                    , _filePath.c_str()
                    );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DevelopmentHotkeyPlugin::updateDebugResourceCompile_()
    {
        if( m_debugResourceCompile == true )
        {
            NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_COMPILE, &DevelopmentHotkeyPlugin::notifyDebugResourceCompile_, MENGINE_DOCUMENT_FACTORABLE );
            NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_RELEASE, &DevelopmentHotkeyPlugin::notifyDebugResourceRelease_, MENGINE_DOCUMENT_FACTORABLE );
        }
        else
        {
            NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_COMPILE );
            NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_RELEASE );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DevelopmentHotkeyPlugin::notifyDebugResourceCompile_( Resource * _resource )
    {
        LOGGER_VERBOSE_LEVEL( "debug", LM_MESSAGE_RELEASE, LFILTER_NONE, LCOLOR_GREEN, nullptr, 0, nullptr, LFLAG_SHORT )("compile %s type %s [%s]"
            , _resource->getName().c_str()
            , Helper::getFactorableType( _resource ).c_str()
            , _resource->getGroupName().c_str()
            );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevelopmentHotkeyPlugin::notifyDebugResourceRelease_( Resource * _resource )
    {
        LOGGER_VERBOSE_LEVEL( "debug", LM_MESSAGE_RELEASE, LFILTER_NONE, LCOLOR_GREEN, nullptr, 0, nullptr, LFLAG_SHORT )("release %s type %s [%s]"
            , _resource->getName().c_str()
            , Helper::getFactorableType( _resource ).c_str()
            , _resource->getGroupName().c_str()
            );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevelopmentHotkeyPlugin::reloadLocale_()
    {
        const ConstString & locale = APPLICATION_SERVICE()
            ->getLocale();

        LOGGER_MESSAGE_RELEASE( "reload locale [%s]"
            , locale.c_str()
        );

        DevelopmentHotkeyReloadLocaleSceneChangePtr cb = Helper::makeFactorableUnique<DevelopmentHotkeyReloadLocaleSceneChange>( MENGINE_DOCUMENT_FACTORABLE );

        SCENE_SERVICE()
            ->restartCurrentScene( false, cb );
    }
    //////////////////////////////////////////////////////////////////////////
}
