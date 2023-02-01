#pragma once

#include "DevToDebugInterface.h"

#include "Plugins/cURLPlugin/cURLInterface.h"
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
        EDTDS_CONNECTING,
        EDTDS_CONNECT,
    };

    class DevToDebugService
        : public ServiceBase<DevToDebugServiceInterface>
        , public cURLReceiverInterface
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

    protected:
        void onHttpRequestComplete( const cURLResponseInterfacePtr & _response ) override;

    protected:
        void notifyBootstrapperRunComplete_();
        void notifyBootstrapperFinalizeGame_();

    protected:
        jpp::object makeJsonTabs( bool _force, bool * const _invalidateTabs );
        jpp::object makeJsonConnectData();
        jpp::object makeJsonProcessData();

    protected:
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