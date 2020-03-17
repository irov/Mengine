#pragma once

#include "Interface/ApplicationInterface.h"

#include "Interface/NotificationServiceInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/SoundSystemInterface.h"
#include "Interface/SoundServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/CodecInterface.h"
#include "Interface/DataInterface.h"
#include "Interface/ConverterInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/PluginInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/TimeSystemInterface.h"

#include "Kernel/Map.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32Application
    {
    public:
        Win32Application();
        ~Win32Application();

    public:
        bool initialize();
        void finalize();

    public:
        void loop();

    protected:
        bool initializeFileService_();
        bool initializeUserDirectory_();
        bool initializeLoggerFile_();
        bool initializeOptionsService_();
        bool initializeLoggerService_();
        bool initializeArchiveService_();

    protected:
        ServiceProviderInterface * m_serviceProvider;

        LoggerInterfacePtr m_loggerFile;
        LoggerInterfacePtr m_loggerMessageBox;
    };
}
