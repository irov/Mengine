#pragma once

#include "Interface/ApplicationInterface.h"

#include "Interface/StringizeInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/UnicodeInterface.h"
#include "Interface/ArchiveInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/PhysicSystemInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/RenderNodeServiceInterface.h"
#include "Interface/SoundSystemInterface.h"
#include "Interface/ScriptSystemInterface.h"
#include "Interface/ParticleSystemInterface.h"
#include "Interface/CodecInterface.h"
#include "Interface/DataInterface.h"
#include "Interface/ConverterInterface.h"
#include "Interface/InputSystemInterface.h"
#include "Interface/PluginInterface.h"
#include "Interface/WindowsLayerInterface.h"
#include "Interface/HttpSystemInterface.h"
#include "Interface/PrefetcherInterface.h"
#include "Interface/ConfigInterface.h"
#include "Interface/TimerInterface.h"

#include "Config/Map.h"

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
        void update();

    public:
        void loop();

    protected:
        bool initializeFileEngine_();
        bool initializeUserDirectory_();
        bool initializeLogFile_();
        bool initializeLogEngine_();
        bool initializeConfigEngine_();
        bool initializeArchiveService_();
        bool initializeModuleEngine_();
        bool initializeDataManager_();
        bool initializeMemoryManager_();
        bool initializeHttpService_();
        bool initializePrefetcherService_();
        bool initializeTimerService_();

    protected:
        bool getApplicationPath_( const char * _section, const char * _key, ConstString & _value );

    protected:
        bool makeUserPath_( WString & _wstring ) const;

    protected:
        ServiceProviderInterface * m_serviceProvider;

        LoggerInterfacePtr m_fileLog;
        LoggerInterfacePtr m_loggerMessageBox;
    };
}
