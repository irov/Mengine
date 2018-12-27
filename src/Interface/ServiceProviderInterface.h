#pragma once

#include "Config/Typedef.h"
#include "Config/Lambda.h"
#include "Config/Char.h"

#include "Kernel/IntrusivePtr.h"
#include "Kernel/Holder.h"
#include "Kernel/Exception.h"
#include "Kernel/Typename.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class ServiceInterface> ServiceInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    typedef bool( *TServiceProviderGenerator )(ServiceInterfacePtr*);
    //////////////////////////////////////////////////////////////////////////
    class ServiceProviderInterface
    {
    public:
        ServiceProviderInterface() noexcept {};
        virtual ~ServiceProviderInterface() {};

    public:
        virtual bool existService( const Char * _name ) const = 0;

    public:
        virtual const ServiceInterfacePtr & getService( const Char * _name ) const = 0;

    public:
        virtual ServiceInterfacePtr generateService( TServiceProviderGenerator _generator ) = 0;

        template<class T>
        IntrusivePtr<T> generateServiceT( TServiceProviderGenerator _generator, const Char * _file, uint32_t _line )
        {
            ServiceInterfacePtr service = this->generateService( _generator );

#ifndef NDEBUG
            const Char * serviceName = T::getStaticServiceID();

            if( service == nullptr )
            {
                MENGINE_THROW_EXCEPTION_FL( _file, _line )("Generate service %s not found"
                    , serviceName
                    );
            }

            if( stdex::intrusive_dynamic_cast<IntrusivePtr<T>>(service) == nullptr )
            {
                MENGINE_THROW_EXCEPTION_FL( _file, _line )("Genereate service %s invalid cast to '%s'"
                    , serviceName
                    , Typename<T>::value
                    );

                throw;
            }
#else
            (void)_file;
            (void)_line;
#endif
            IntrusivePtr<T> t = stdex::intrusive_static_cast<IntrusivePtr<T>>(service);

            return t;
        }

    public:
        virtual bool initializeService( TServiceProviderGenerator _generator, const Char * _doc, const Char * _file, uint32_t _line ) = 0;
        virtual bool finalizeService( const Char * _name ) = 0;
        virtual bool destroyService( const Char * _name ) = 0;

    public:
        virtual void dependencyService( const Char * _name, const Char * _dependency ) = 0;

        typedef Lambda<void( const ServiceInterfacePtr & )> LambdaWaitService;
        virtual void waitService( const Char * _name, const LambdaWaitService & _lambda ) = 0;

    public:
        virtual void stopServices() = 0;

    public:
        template<class T>
        bool finalizeServiceT()
        {
            const Char * name = T::getStaticServiceID();

            bool successful = this->finalizeService( name );

            return successful;
        }

        template<class T>
        bool destroyServiceT()
        {
            const Char * name = T::getStaticServiceID();

            bool successful = this->destroyService( name );

            return successful;
        }

    public:
        virtual void destroy() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#define SERVICE_PROVIDER_SETUP( Ptr )\
    Mengine::Holder<Mengine::ServiceProviderInterface>::keep( (Ptr) )
    //////////////////////////////////////////////////////////////////////////
#define SERVICE_PROVIDER_GET()\
    Mengine::Holder<Mengine::ServiceProviderInterface>::get()
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
	extern bool SERVICE_PROVIDER_NAME_CREATE( Name )(Mengine::ServiceProviderInterface**);
    //////////////////////////////////////////////////////////////////////////
#define SERVICE_PROVIDER_CREATE( Name, Provider )\
	SERVICE_PROVIDER_NAME_CREATE(Name)(Provider)
    //////////////////////////////////////////////////////////////////////////
#define SERVICE_PROVIDER_FINALIZE( Provider )\
	Provider->destroy()
}
