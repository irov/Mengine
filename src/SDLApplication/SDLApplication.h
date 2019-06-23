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

    protected:
        bool loadApplicationConfig_();

    protected:
        bool initializeFileEngine_();
        bool initializeConfigEngine_();
        bool initializeUserDirectory_();
        bool initializeLoggerFile_();
        bool initializeLoggerEngine_();
        bool initializeRenderEngine_();
        bool initializeSoundEngine_();
        bool initializeArchiveService_();

    protected:
        ServiceProviderInterface * m_serviceProvider;

        ThreadMutexInterfacePtr m_mutexAllocatorPool;

        LoggerInterfacePtr m_loggerMessageBox;
        LoggerInterfacePtr m_loggerStdio;
        LoggerInterfacePtr m_fileLog;

        bool m_running;
        bool m_active;

        bool m_developmentMode;

        FilePath m_publicConfigPath;
        FilePath m_privateConfigPath;
        FilePath m_resourceConfigPath;
    };
}
