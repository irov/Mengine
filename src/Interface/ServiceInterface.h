#pragma once

#include "Interface/ServiceProviderInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/InitializerList.h"
#include "Kernel/Assertion.h"
#include "Kernel/ExceptionHelper.h"
#include "Kernel/Typename.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef InitializeList<const Char *> ServiceRequiredList;
    //////////////////////////////////////////////////////////////////////////
    class ServiceInterface
        : public Factorable
    {
    public:
        virtual const Char * getServiceID() const = 0;

    public:
        virtual bool isAvailableService() const = 0;
        virtual bool isInitializeService() const = 0;
        virtual bool isStopService() const = 0;

    public:        
        virtual const ServiceRequiredList & requiredServices() const = 0;

    protected:
        virtual bool availableService() = 0;
        virtual bool initializeService() = 0;
        virtual void finalizeService() = 0;
        virtual void replaceService() = 0;
        virtual bool runService() = 0;
        virtual void stopService() = 0;

    public:
        friend class ServiceProvider;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ServiceInterface> ServiceInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            template<class T>
            T * getService2( const Char * _file, uint32_t _line )
            {
                MENGINE_UNUSED( _file );
                MENGINE_UNUSED( _line );

                const Char * serviceName = T::getStaticServiceID();

                ServiceProviderInterface * serviceProvider = SERVICE_PROVIDER_GET();

#if defined(MENGINE_DEBUG)
                if( serviceProvider == nullptr )
                {
                    MENGINE_THROW_EXCEPTION_FL( _file, _line )("Service '%s' invalid get provider"
                        , serviceName
                        );
                    
                    return nullptr;
                }
#endif

                const ServiceInterfacePtr & service = serviceProvider->getService( serviceName );

                ServiceInterface * service_ptr = service.get();

#if defined(MENGINE_DEBUG)
                if( service_ptr == nullptr )
                {
                    MENGINE_THROW_EXCEPTION_FL( _file, _line )("Service '%s' not found"
                        , serviceName
                        );
                    
                    return nullptr;
                }
#endif

#if defined(MENGINE_DEBUG)
                if( dynamic_cast<T *>(service_ptr) == nullptr )
                {
                    MENGINE_THROW_EXCEPTION_FL( _file, _line )("Service '%s' invalid cast to '%s'"
                        , serviceName
                        , Typename<T>::value
                        );
                    
                    return nullptr;
                }
#endif

                T * service_ptr_t = static_cast<T *>(service_ptr);

                return service_ptr_t;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T * getService( const Char * _file, uint32_t _line )
        {
            MENGINE_UNUSED( _file );
            MENGINE_UNUSED( _line );

            static T * s_service = Detail::getService2<T>( _file, _line );

            MENGINE_ASSERTION_FATAL( s_service == nullptr || s_service->isInitializeService() == true, "service '%s' not initialized"
                , T::getStaticServiceID()
            );

            return s_service;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        bool isExistService()
        {
            static const bool * s_exist = SERVICE_PROVIDER_GET()
                ->isExistServiceProvider( T::getStaticServiceID() );

            return *s_exist;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        bool isAvailableService()
        {
            static const bool * s_available = SERVICE_PROVIDER_GET()
                ->isAvailableServiceProvider( T::getStaticServiceID() );

            return *s_available;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        bool isInitializeService()
        {
            static const bool * s_initialize = SERVICE_PROVIDER_GET()
                ->isInitializeServiceProvider( T::getStaticServiceID() );

            return *s_initialize;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
#define SERVICE_GET( Type )\
    (Mengine::Helper::getService<Type>(MENGINE_CODE_FILE, MENGINE_CODE_LINE))
//////////////////////////////////////////////////////////////////////////
#define SERVICE_IS_EXIST( Type )\
    (Mengine::Helper::isExistService<Type>())
//////////////////////////////////////////////////////////////////////////
#define SERVICE_IS_AVAILABLE( Type )\
    (Mengine::Helper::isAvailableService<Type>())
//////////////////////////////////////////////////////////////////////////
#define SERVICE_IS_INITIALIZE( Type )\
    (Mengine::Helper::isInitializeService<Type>())
//////////////////////////////////////////////////////////////////////////
#define SERVICE_FUNCTION_CREATE(Name)\
    __createMengineService##Name
//////////////////////////////////////////////////////////////////////////
#define SERVICE_FUNCTION_NOMINATION(Name)\
    __nominationMengineService##Name
//////////////////////////////////////////////////////////////////////////
#define SERVICE_DECLARE( ID )\
    public:\
        MENGINE_INLINE static const Mengine::Char * getStaticServiceID(){ return ID; };\
        MENGINE_INLINE const Mengine::Char * getServiceID() const override { return ID; };\
    protected:
//////////////////////////////////////////////////////////////////////////
#define SERVICE_FACTORY( Name, Implement )\
    bool SERVICE_FUNCTION_CREATE(Name)(Mengine::ServiceInterface**_service){\
    if(_service==nullptr){return false;}\
    try{*_service=new Implement();}catch(...){return false;}\
    return true;}\
    const Mengine::Char * SERVICE_FUNCTION_NOMINATION(Name)(){\
    return Implement::getStaticServiceID();}\
    struct __mengine_dummy_factory##Name{}
//////////////////////////////////////////////////////////////////////////
#define SERVICE_DEPENDENCY( Type, Dependency )\
    SERVICE_PROVIDER_GET()->dependencyService(Type::getStaticServiceID(), Dependency::getStaticServiceID())
//////////////////////////////////////////////////////////////////////////
#define SERVICE_EXTERN( Name )\
    extern bool SERVICE_FUNCTION_CREATE( Name )(Mengine::ServiceInterface**);\
    extern const Mengine::Char * SERVICE_FUNCTION_NOMINATION( Name )()
//////////////////////////////////////////////////////////////////////////
#define SERVICE_CREATE( Name, Doc )\
    SERVICE_PROVIDER_GET()->createService(&SERVICE_FUNCTION_CREATE(Name), false, Doc)
//////////////////////////////////////////////////////////////////////////
#define SERVICE_CREATE_SAFE( Name, Doc )\
    SERVICE_PROVIDER_GET()->createService(&SERVICE_FUNCTION_CREATE(Name), true, Doc)
//////////////////////////////////////////////////////////////////////////
#define SERVICE_FINALIZE( Name )\
    SERVICE_PROVIDER_GET()->finalizeService(SERVICE_FUNCTION_NOMINATION(Name)())
//////////////////////////////////////////////////////////////////////////
#define SERVICE_DESTROY( Name )\
    SERVICE_PROVIDER_GET()->destroyService(SERVICE_FUNCTION_NOMINATION(Name)())
//////////////////////////////////////////////////////////////////////////
#define UNKNOWN_SERVICE_WAIT( Owner, Type, Lambda )\
    SERVICE_PROVIDER_GET()->waitService(#Owner, Type::getStaticServiceID(), Lambda)
//////////////////////////////////////////////////////////////////////////
#define UNKNOWN_SERVICE_LEAVE( Owner, Type, Lambda )\
    SERVICE_PROVIDER_GET()->leaveService(#Owner, Type::getStaticServiceID(), Lambda)
//////////////////////////////////////////////////////////////////////////
#define UNKNOWN_SERVICE_UNLINK( Owner )\
    SERVICE_PROVIDER_GET()->unlinkService(#Owner)
//////////////////////////////////////////////////////////////////////////
#define SERVICE_WAIT( Type, Lambda )\
    SERVICE_PROVIDER_GET()->waitService(this->getServiceID(), Type::getStaticServiceID(), Lambda)
//////////////////////////////////////////////////////////////////////////
#define SERVICE_WAIT_METHOD( Type, Self, Method )\
    SERVICE_PROVIDER_GET()->waitService(this->getServiceID(), Type::getStaticServiceID(), [Self](){return Self->Method();})
//////////////////////////////////////////////////////////////////////////
#define SERVICE_LEAVE( Type, Lambda )\
    SERVICE_PROVIDER_GET()->leaveService(this->getServiceID(), Type::getStaticServiceID(), Lambda)
//////////////////////////////////////////////////////////////////////////
#define SERVICE_LEAVE_METHOD( Type, Self, Method )\
    SERVICE_PROVIDER_GET()->leaveService(this->getServiceID(), Type::getStaticServiceID(), [Self](){Self->Method();})
//////////////////////////////////////////////////////////////////////////
