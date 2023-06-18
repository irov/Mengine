#pragma once

#include "Interface/RenderTextureInterface.h"
#include "Interface/RenderPipelineInterface.h"
#include "Interface/HttpSystemInterface.h"
#include "Interface/FactoryInterface.h"

#include "Kernel/Tuple.h"
#include "Kernel/ConstString.h"
#include "Kernel/Tags.h"
#include "Kernel/Factorable.h"
#include "Kernel/Resolution.h"
#include "Kernel/LoggerLevel.h"
#include "Kernel/AssertionLevel.h"
#include "Kernel/ErrorLevel.h"
#include "Kernel/LoggerMessage.h"

#include "Config/Typedef.h"
#include "Config/Char.h"
#include "Config/UniqueId.h"

#ifndef MENGINE_NOTIFICATOR_MAX_COUNT
#define MENGINE_NOTIFICATOR_MAX_COUNT 256
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Resource> ResourcePtr;
    typedef IntrusivePtr<class Arrow, class Node> ArrowPtr;
    typedef IntrusivePtr<class Scene, class Node> ScenePtr;
    //////////////////////////////////////////////////////////////////////////
    template<uint32_t ID>
    struct Notificator;
    //////////////////////////////////////////////////////////////////////////
#define MENGINE_NOTIFICATOR_DECLARE_BEGIN()\
    static constexpr uint32_t MENGINE_NOTIFICATOR_ENUMERATOR_BEGIN = MENGINE_CODE_LINE
        //////////////////////////////////////////////////////////////////////////
#define MENGINE_NOTIFICATOR_DECLARE_END()\
    static constexpr uint32_t MENGINE_NOTIFICATOR_ENUMERATOR_COUNT = MENGINE_CODE_LINE - MENGINE_NOTIFICATOR_ENUMERATOR_BEGIN - 1;\
    static_assert(MENGINE_NOTIFICATOR_ENUMERATOR_COUNT < MENGINE_NOTIFICATOR_MAX_COUNT)
    //////////////////////////////////////////////////////////////////////////
#define MENGINE_NOTIFICATOR_DECLARE(NAME, ...)\
    static constexpr uint32_t NAME = MENGINE_CODE_LINE - MENGINE_NOTIFICATOR_ENUMERATOR_BEGIN;\
    template<> struct Notificator<NAME> { typedef Tuple<__VA_ARGS__> args_type; static const Char * getName() { return #NAME; } }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_NOTIFICATOR_DECLARE_BEGIN();
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_DEBUG_OPEN_FILE, const Char *, const Char *, bool );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_DEBUG_CLOSE_FILE, const Char *, const Char *, bool );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_ASSERTION, const Char *, EAssertionLevel, const Char *, const Char *, int32_t, const Char * );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_ABORT, const Char * );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_CRASH, const Char * );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_ERROR, const Char *, EErrorLevel, const Char *, int32_t, const Char * );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, bool, Resolution );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_CREATE_RENDER_WINDOW );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_RENDER_SCENE_BEGIN, const RenderPipelineInterfacePtr & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_RENDER_SCENE_END, const RenderPipelineInterfacePtr & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_RENDER_DEVICE_LOST_PREPARE );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_RENDER_DEVICE_LOST_RESTORE );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_DEVELOPMENT_RESOURCE_INITIALIZE, Resource * );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_DEVELOPMENT_RESOURCE_COMPILE, Resource * );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_DEVELOPMENT_RESOURCE_RELEASE, Resource * );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_CHANGE_ARROW_PREPARE_DESTROY, const ArrowPtr & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_CHANGE_ARROW_COMPLETE, const ArrowPtr & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_CHANGE_SCENE_PREPARE_DESTROY, const ScenePtr &, const ScenePtr & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_CHANGE_SCENE_DESTROY, const ScenePtr & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_CHANGE_SCENE_PREPARE_INITIALIZE, const ScenePtr & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_CHANGE_SCENE_INITIALIZE, const ScenePtr & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_CHANGE_SCENE_PREPARE_ENABLE, const ScenePtr & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_CHANGE_SCENE_ENABLE, const ScenePtr & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_CHANGE_SCENE_ERROR, const ScenePtr & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_CHANGE_SCENE_ENABLE_FINALLY, const ScenePtr & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_CHANGE_SCENE_PREPARE_COMPLETE, const ScenePtr & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_CHANGE_SCENE_COMPLETE, const ScenePtr & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_RESTART_SCENE_PREPARE_DISABLE, const ScenePtr & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_RESTART_SCENE_DISABLE, const ScenePtr & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_RESTART_SCENE_INITIALIZE, const ScenePtr & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_RESTART_SCENE_PREPARE_ENABLE, const ScenePtr & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_RESTART_SCENE_ERROR, const ScenePtr & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_RESTART_SCENE_ENABLE, const ScenePtr & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_RESTART_SCENE_ENABLE_FINALLY, const ScenePtr & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_RESTART_SCENE_PREPARE_COMPLETE, const ScenePtr & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_RESTART_SCENE_COMPLETE, const ScenePtr & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_REMOVE_SCENE_PREPARE_DESTROY, const ScenePtr & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_REMOVE_SCENE_DESTROY );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_REMOVE_SCENE_PREPARE_COMPLETE );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_REMOVE_SCENE_COMPLETE );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_CHANGE_LOCALE_PREPARE, const ConstString &, const ConstString & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_CHANGE_LOCALE, const ConstString &, const ConstString & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_CHANGE_LOCALE_POST, const ConstString &, const ConstString & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_DEBUG_TEXT_MODE, bool );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_CHANGE_TEXT_ALIAS, ConstString, ConstString );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_RELOAD_LOCALE_PREPARE );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_RELOAD_LOCALE );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_RELOAD_LOCALE_POST );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_PLATFORM_RUN );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_PLATFORM_READY );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_PLATFORM_UPDATE );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_PLATFORM_STOP );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_ENGINE_STOP );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_ENGINE_PREPARE_FINALIZE );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_ENGINE_FINALIZE );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_ENGINE_STOP_SERVICES );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_ENGINE_STOP_THREADS );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_ENABLE_PACKAGES, const ConstString & , const Tags & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_RENDER_TEXTURE_DESTROY, RenderTextureInterface * );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_SCRIPT_EMBEDDING );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_SCRIPT_EMBEDDING_END );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_SCRIPT_EJECTING );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_TURN_SOUND, bool );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_BASE_SERVICES );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_BOOTSTRAPPER_RUN_FRAMEWORKS );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_BOOTSTRAPPER_RUN_COMPLETE );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_GAME );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_BOOTSTRAPPER_FINALIZE_GAME );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_APPLICATION_BEGIN_UPDATE );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_APPLICATION_END_UPDATE );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_MODULE_INITIALIZE, ConstString );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_MODULE_FINALIZE, ConstString );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_PLUGIN_INITIALIZE, const Char * );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_PLUGIN_FINALIZE, const Char * );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_INCREF_FACTORY_GENERATION, uint32_t  );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_LOGGER_BEGIN, const LoggerMessage & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_LOGGER_END, const LoggerMessage & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_TIME_FACTOR_CHANGE, float );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_PACKAGES_LOAD );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_CONFIGS_LOAD );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_ACCOUNTS_LOAD );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_PLATFORM_ATACH_WINDOW );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_PLATFORM_DETACH_WINDOW );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_HTTP_REQUEST, HttpRequestId, const String & );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_HTTP_CANCEL, HttpRequestId );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_HTTP_RESPONSE, const HttpResponseInterfacePtr & );

#if defined(MENGINE_PLATFORM_IOS)
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_IOS_APPLICATION_DID_BECOME_ACTIVE );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_IOS_APPLICATION_WILL_ENTER_FOREGROUND );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_IOS_APPLICATION_DID_ENTER_BACKGROUD );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_IOS_APPLICATION_WILL_RESIGN_ACTIVE );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_IOS_APPLICATION_WILL_TERMINATE );
    MENGINE_NOTIFICATOR_DECLARE( NOTIFICATOR_IOS_APPLICATION_DID_RECEIVE_MEMORY_WARNING );
#endif
    MENGINE_NOTIFICATOR_DECLARE_END();
    //////////////////////////////////////////////////////////////////////////
#undef MENGINE_NOTIFICATOR_DECLARE_BEGIN
#undef MENGINE_NOTIFICATOR_DECLARE_END
#undef MENGINE_NOTIFICATOR_DECLARE
    //////////////////////////////////////////////////////////////////////////
}
