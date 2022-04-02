#pragma once

#include "DevToDebugInterface.h"

#include "Plugins/cURLPlugin/cURLInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Hashtable.h"

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
        void addProvider( const ConstString & _type, const DevToDebugProviderInterfacePtr & _provider ) override;
        void removeProvider( const ConstString & _type ) override;

    protected:
        void process();

    protected:
        void onHttpRequestComplete( const cURLResponseData & _response ) override;

    protected:
        void notifyBootstrapperRunComplete_();


    protected:
        typedef Hashtable<ConstString, DevToDebugProviderInterfacePtr> HashtableDevToDebugProviders;
        HashtableDevToDebugProviders m_providers;

        uint32_t m_status;

        String m_pid;
    };
}