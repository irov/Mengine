#pragma once

#include "Kernel/IntrusivePtr.h"
#include "Kernel/Holder.h"
#include "Kernel/Exception.h"
#include "Kernel/Document.h"
#include "Kernel/Typename.h"

#include "Config/Typedef.h"
#include "Config/Lambda.h"
#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class ServiceInterface> ServiceInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    typedef bool(*FServiceProviderGenerator)(ServiceInterface **);
    //////////////////////////////////////////////////////////////////////////
    class ServiceProviderInterface
    {
    public:
        ServiceProviderInterface() noexcept
        {
        };

        virtual ~ServiceProviderInterface()
        {
        };

    public:
        virtual const bool * isExistServiceProvider( const Char * _name ) = 0;
        virtual const bool * isAvailableServiceProvider( const Char * _name ) = 0;
        virtual const bool * isInitializeServiceProvider( const Char * _name ) = 0;

    public:
        virtual const ServiceInterfacePtr & getService( const Char * _name ) const = 0;

    public:
        virtual bool createService( FServiceProviderGenerator _generator, bool _safe, const DocumentPtr & _doc ) = 0;
        virtual bool finalizeService( const Char * _name ) = 0;
        virtual bool destroyService( const Char * _name ) = 0;

    public:
        virtual void dependencyService( const Char * _name, const Char * _dependency ) = 0;

    public:
        typedef Lambda<bool()> LambdaWaitService;
        virtual bool waitService( const Char * _owner, const Char * _name, const LambdaWaitService & _lambda ) = 0;

        typedef Lambda<void()> LambdaLeaveService;
        virtual bool leaveService( const Char * _owner, const Char * _name, const LambdaLeaveService & _lambda ) = 0;

        virtual void unlinkService( const Char * _owner ) = 0;

    public:
        virtual void stopServices() = 0;

    public:
        virtual void destroy() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define SERVICE_PROVIDER_SETUP( Ptr )\
    Mengine::Holder<Mengine::ServiceProviderInterface>::keep( (Ptr) )
//////////////////////////////////////////////////////////////////////////
#define SERVICE_PROVIDER_GET()\
    Mengine::Holder<Mengine::ServiceProviderInterface>::get()
//////////////////////////////////////////////////////////////////////////
#define SERVICE_PROVIDER_PUT()\
    Mengine::Holder<Mengine::ServiceProviderInterface>::put()
//////////////////////////////////////////////////////////////////////////
#define SERVICE_PROVIDER_EXIST()\
    Mengine::Holder<Mengine::ServiceProviderInterface>::isKeep()
//////////////////////////////////////////////////////////////////////////
#define SERVICE_PROVIDER_STOP()\
    SERVICE_PROVIDER_GET()->stopServices()
//////////////////////////////////////////////////////////////////////////
#define SERVICE_PROVIDER_NAME_CREATE(Name)\
    __createMengineProvider##Name
//////////////////////////////////////////////////////////////////////////
#define SERVICE_PROVIDER_FACTORY( Name, Implement )\
    bool SERVICE_PROVIDER_NAME_CREATE(Name)(Mengine::ServiceProviderInterface**_serviceProvider){\
    if(_serviceProvider==nullptr){return false;}\
    try{*_serviceProvider=new Implement();}catch(...){return false;}\
    return true;}\
    struct __mengine_dummy_factory##Name{}
//////////////////////////////////////////////////////////////////////////
#define SERVICE_PROVIDER_EXTERN(Name)\
    extern bool SERVICE_PROVIDER_NAME_CREATE( Name )(Mengine::ServiceProviderInterface**)
//////////////////////////////////////////////////////////////////////////
#define SERVICE_PROVIDER_CREATE( Name, Provider )\
    SERVICE_PROVIDER_NAME_CREATE(Name)(Provider)
//////////////////////////////////////////////////////////////////////////
#define SERVICE_PROVIDER_FINALIZE( Provider )\
    Provider->destroy()
//////////////////////////////////////////////////////////////////////////
