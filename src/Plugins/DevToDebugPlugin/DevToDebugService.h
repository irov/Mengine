#pragma once

#include "DevToDebugInterface.h"

#include "Plugins/cURLPlugin/cURLInterface.h"
#include "Plugins/JSONPlugin/JSONInterface.h"

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

    protected:
        void addTab( const ConstString & _name, const DevToDebugTabInterfacePtr & _tab ) override;
        const DevToDebugTabInterfacePtr & getTab( const ConstString & _name ) const override;
        bool hasTab( const ConstString & _name ) const override;
        void removeTab( const ConstString & _name ) override;

    protected:
        void process();
        void stop();

    protected:
        void onHttpRequestComplete( const cURLResponseData & _response ) override;

    protected:
        void notifyBootstrapperRunComplete_();
        void notifyBootstrapperFinalizeGame_();

    protected:
        jpp::object makeConnectData();
        jpp::object makeProcessData();

    protected:
        typedef Hashtable<ConstString, DevToDebugTabInterfacePtr> HashtableDevToDebugTabs;
        HashtableDevToDebugTabs m_tabs;

        EDevToDebugStatus m_status;

        String m_pid;
        String m_uuid;

        uint32_t m_revision;

        UniqueId m_timerId;
    };
}