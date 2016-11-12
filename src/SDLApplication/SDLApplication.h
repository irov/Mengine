#pragma once

#   include "Interface/ApplicationInterface.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/NotificationServiceInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/ThreadSystemInterface.h"
#   include "Interface/RenderSystemInterface.h"
#   include "Interface/SoundSystemInterface.h"
#   include "Interface/PhysicSystemInterface.h"
#   include "Interface/ScriptSystemInterface.h"
#   include "Interface/ParticleSystemInterface.h"
#   include "Interface/CodecInterface.h"
#   include "Interface/ConverterInterface.h"
#   include "Interface/InputSystemInterface.h"
#   include "Interface/PluginInterface.h"
#   include "Interface/ArchiveInterface.h"
#   include "Interface/ModuleInterface.h"
#   include "Interface/DataInterface.h"
#   include "Interface/MemoryInterface.h"
#   include "Interface/ConfigInterface.h"
#   include "Interface/PrefetcherInterface.h"

#   include "SDLLogger.h"

#   include "Core/FileLogger.h"

namespace Menge
{

    class SDLApplication
    {
    public:
        SDLApplication();
        ~SDLApplication();

    public:
        bool initialize(const int argc, char** argv);
        void finalize();

    public:
        void loop();
        void stop();

    protected:
        bool getApplicationPath_(const char * _section, const char * _key, ConstString & _path);

    protected:
        bool initializeStringizeService_();
        bool initializeApplicationService_();
        bool initializeNotificationService_();
        bool initializeThreadEngine_();
        bool initializeFileEngine_();
        bool initializeConfigEngine_();
        bool initializeLoggerEngine_();
        bool initializeUnicodeEngine_();
        bool initializeParticleEngine2_();
        bool initializePhysicEngine2D_();
        bool initializeRenderEngine_();
        bool initializeSoundEngine_();
        bool initializeSilentSoundEngine_();
        bool initializeNodeManager_();
        bool initializeScriptEngine_();
        bool initializeCodecEngine_();
        bool initializeInputEngine_();
        bool initializePluginService_();
        bool initializeArchiveService_();
        bool initializeModuleEngine_();
        bool initializeDataManager_();
        bool initializeMemoryManager_();
        bool initializePrefetcherService_();

    public:
        void setActivate(bool _value);

    protected:
        ServiceProviderInterface * m_serviceProvider;

        SDLLogger * m_loggerConsole;
        FileLogger * m_fileLog;

        ConstString m_platformName;

        FilePath m_currentPath;
        WString m_userPath;
        WString m_tempPath;

        WString m_projectName;
        WString m_companyName;

        typedef stdex::vector<PluginInterface *> TVectorPlugins;
        TVectorPlugins m_plugins;

        bool m_running;
        bool m_active;

        bool m_developmentMode;
    };
}
