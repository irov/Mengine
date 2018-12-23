#pragma once

#include "Interface/ApplicationInterface.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/SoundSystemInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/CodecInterface.h"
#include "Interface/ConverterInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/PluginInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/ModuleInterface.h"
#include "Interface/DataInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/PrefetcherServiceInterface.h"

#include "Kernel/FileLogger.h"

namespace Mengine
{

    class SDLApplication
    {
    public:
        SDLApplication();
        ~SDLApplication();

    public:
        bool initialize( const int argc, char** argv );
        void finalize();

    public:
        void loop();
        void stop();

    protected:
        bool getApplicationPath_( const Char * _section, const Char * _key, ConstString & _path );

    protected:
        bool initializeStringizeService_();
        bool initializeApplicationService_();
        bool initializeNotificationService_();
        bool initializeThreadEngine_();
        bool initializeFileEngine_();
        bool initializeConfigEngine_();
        bool initializeUserDirectory_();
        bool initializeLoggerFile_();
        bool initializeLoggerEngine_();
        bool initializeUnicodeEngine_();
        bool initializeParticleEngine2_();
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
        void setActivate( bool _value );

    protected:
        ServiceProviderInterface * m_serviceProvider;

        LoggerInterfacePtr m_loggerMessageBox;
        LoggerInterfacePtr m_loggerStdio;
        LoggerInterfacePtr m_fileLog;

        ConstString m_platformName;

        WString m_projectName;
        WString m_companyName;

        bool m_running;
        bool m_active;

        bool m_developmentMode;
    };
}
