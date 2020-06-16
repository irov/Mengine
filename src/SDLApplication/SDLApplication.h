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
        bool initialize( int32_t _argc, Char ** const _argv );
        void finalize();

    public:
        void loop();

    protected:
        bool initializeOptionsService_( int32_t argc, Char ** const _argv );
        bool initializeFileService_();
        bool initializeLoggerService_();

    protected:
        void finalizeLoggerService_();

    protected:
        ServiceProviderInterface * m_serviceProvider;

        LoggerInterfacePtr m_loggerMessageBox;
        LoggerInterfacePtr m_loggerStdio;

        bool m_running;
        bool m_active;

        bool m_developmentMode;
    };
}
