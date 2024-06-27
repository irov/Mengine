#pragma once

#include "DevToDebugInterface.h"

#include "Interface/ThreadWorkerInterface.h"
#include "Interface/HttpSystemInterface.h"

#include "Plugins/JSONPlugin/JSONInterface.h"

#include "DevToDebugLogger.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Hashtable.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    enum EDevToDebugStatus
    {
        EDTDS_NONE,
        EDTDS_READY,
        EDTDS_REGISTRATING,
        EDTDS_REGISTRATION,
        EDTDS_WAITING,
        EDTDS_WAIT,
        EDTDS_CONNECTING,
        EDTDS_CONNECT,
    };

    class DevToDebugService
        : public ServiceBase<DevToDebugServiceInterface>
        , public ThreadWorkerInterface
        , public HttpReceiverInterface
    {
    public:
        DevToDebugService();
        ~DevToDebugService() override;

    public:
        const ServiceRequiredList & requiredServices() const override;
        bool _initializeService() override;
        void _finalizeService() override;
        void _stopService() override;

    protected:
        void addTab( const ConstString & _name, const DevToDebugTabInterfacePtr & _tab ) override;
        const DevToDebugTabInterfacePtr & getTab( const ConstString & _name ) const override;
        bool hasTab( const ConstString & _name ) const override;
        void removeTab( const ConstString & _name ) override;

    protected:
        void process();
        void sync();
        void stop();

    public:
        void onThreadWorkerUpdate( UniqueId _id ) override;
        bool onThreadWorkerWork( UniqueId _id ) override;
        void onThreadWorkerDone( UniqueId _id ) override;

    protected:
        void onHttpRequestComplete( const HttpResponseInterfacePtr & _response ) override;

    protected:
        void notifyBootstrapperRunComplete_();
        void notifyBootstrapperFinalizeGame_();

    protected:
        jpp::object makeJsonTabs( bool _force, bool * const _invalidateTabs );
        jpp::object makeJsonRegistrationData();
        jpp::object makeJsonWaitData();
        jpp::object makeJsonConnectData();
        jpp::object makeJsonProcessData();

    protected:
        Char m_fingerprint[MENGINE_SHA1_HEX_COUNT + 1];

        ThreadMutexInterfacePtr m_mutexTabs;
        ThreadMutexInterfacePtr m_mutexCommands;

        typedef Hashtable<ConstString, DevToDebugTabInterfacePtr> HashtableDevToDebugTabs;
        HashtableDevToDebugTabs m_tabsProcess;
        HashtableDevToDebugTabs m_tabsSync;

        VectorDevToDebugWidgetCommands m_tabsCommands;

        DevToDebugLoggerPtr m_logger;

        AtomicBool m_invalidateTabs;

        EDevToDebugStatus m_status;

        String m_dsn;
        String m_uuid;
        String m_workerURL;

        uint32_t m_throttle;

        uint32_t m_revision;

        UniqueId m_timerId;
    };
}