#pragma once

#include "Interface/ServiceProviderInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Assertion.h"
#include "Kernel/Typename.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ServiceInterface
        : public Factorable
    {
    public:
        virtual const Char * getServiceID() const = 0;

    protected:
        virtual bool initializeService() = 0;
        virtual void finalizeService() = 0;

    public:
        virtual bool isInitializeService() const = 0;

    public:
        virtual bool isAvailableService() const = 0;

    protected:
        virtual void stopService() = 0;
        virtual bool isStopService() const = 0;

    public:
        friend class ServiceProvider;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ServiceInterface> ServiceInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T * getService( const Char * _file, uint32_t _line )
        {
            MENGINE_UNUSED( _file );
            MENGINE_UNUSED( _line );

            static T * s_service = nullptr;

            if( s_service == nullptr )
            {
                const Char * serviceName = T::getStaticServiceID();

                ServiceProviderInterface * serviceProvider = SERVICE_PROVIDER_GET();

                if( serviceProvider == nullptr )
                {
                    MENGINE_THROW_EXCEPTION_FL( _file, _line )("Service '%s' invalid get provider"
                        , serviceName
                        );

                    return nullptr;
                }

                const ServiceInterfacePtr & service = serviceProvider->getService( serviceName );

                ServiceInterface * service_ptr = service.get();

                if( service_ptr == nullptr )
                {
                    MENGINE_THROW_EXCEPTION_FL( _file, _line )("Service '%s' not found"
                        , serviceName
                        );

                    return nullptr;
                }

#ifdef MENGINE_DEBUG
                if( dynamic_cast<T *>(service_ptr) == nullptr )
                {
                    MENGINE_THROW_EXCEPTION_FL( _file, _line )("Service '%s' invalid cast to '%s'"
                        , serviceName
                        , Typename<T>::value
                        );

                    return nullptr;
                }
#endif

                s_service = static_cast<T *>(service_ptr);
            }

            MENGINE_ASSERTION( s_service == nullptr || s_service->isInitializeService() == true, "service '%s' not initialized"
                , T::getStaticServiceID()
            );

            return s_service;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        bool existService()
        {
            static bool s_exist = false;

            if( s_exist == false )
            {
                const Char * serviceName = T::getStaticServiceID();

                if( SERVICE_PROVIDER_GET()
                    ->existService( serviceName ) == false )
                {
                    s_exist = false;
                }
                else
                {
                    s_exist = true;
                }
            }

            return s_exist;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
#define SERVICE_GET( Type )\
	(Mengine::Helper::getService<Type>(MENGINE_CODE_FILE, MENGINE_CODE_LINE))
//////////////////////////////////////////////////////////////////////////
#define SERVICE_EXIST( Type )\
	(Mengine::Helper::existService<Type>())
//////////////////////////////////////////////////////////////////////////
#define SERVICE_IS_INITIALIZE( Type )\
	(SERVICE_EXIST( Type ) == true && SERVICE_PROVIDER_GET()->getService( Type::getStaticServiceID() )->isInitializeService() == true)
//////////////////////////////////////////////////////////////////////////
#define SERVICE_FUNCTION_CREATE(Name)\
	__createMengineService##Name
//////////////////////////////////////////////////////////////////////////
#define SERVICE_FUNCTION_NOMINATION(Name)\
	__nominationMengineService##Name
//////////////////////////////////////////////////////////////////////////
#define SERVICE_DECLARE( ID )\
    public:\
        MENGINE_INLINE static const Char * getStaticServiceID(){ return ID; };\
		MENGINE_INLINE const Char * getServiceID() const override { return ID; };\
    protected:
//////////////////////////////////////////////////////////////////////////
#define SERVICE_FACTORY( Name, Implement )\
    bool SERVICE_FUNCTION_CREATE(Name)(Mengine::ServiceInterfacePtr*_service){\
    if(_service==nullptr){return false;}\
	try{*_service=new Implement();}catch(...){return false;}\
    return true;}\
    const Mengine::Char * SERVICE_FUNCTION_NOMINATION(Name)(){\
    return Implement::getStaticServiceID();}\
	struct __mengine_dummy_factory##Name{}
//////////////////////////////////////////////////////////////////////////
#define SERVICE_DEPENDENCY( Type, Dependency )\
    SERVICE_PROVIDER_GET()->dependencyService(Type::getStaticServiceID(), SERVICE_GET(Dependency)->getServiceID())
//////////////////////////////////////////////////////////////////////////
#define SERVICE_EXTERN( Name )\
	extern bool SERVICE_FUNCTION_CREATE( Name )(Mengine::ServiceInterfacePtr*);\
    extern const Mengine::Char * SERVICE_FUNCTION_NOMINATION( Name )()
//////////////////////////////////////////////////////////////////////////
#define SERVICE_CREATE( Name )\
	SERVICE_PROVIDER_GET()->initializeService(&SERVICE_FUNCTION_CREATE(Name), false, #Name, MENGINE_CODE_FILE, MENGINE_CODE_LINE)
//////////////////////////////////////////////////////////////////////////
#define SERVICE_CREATE_SAFE( Name )\
	SERVICE_PROVIDER_GET()->initializeService(&SERVICE_FUNCTION_CREATE(Name), true, #Name, MENGINE_CODE_FILE, MENGINE_CODE_LINE)
//////////////////////////////////////////////////////////////////////////
#define SERVICE_FINALIZE( Name )\
	SERVICE_PROVIDER_GET()->finalizeService(SERVICE_FUNCTION_NOMINATION(Name)())
//////////////////////////////////////////////////////////////////////////
#define SERVICE_DESTROY( Name )\
	SERVICE_PROVIDER_GET()->destroyService(SERVICE_FUNCTION_NOMINATION(Name)())
    //////////////////////////////////////////////////////////////////////////
#define SERVICE_WAIT( Type, Lambda )\
    SERVICE_PROVIDER_GET()->waitService(Type::getStaticServiceID(), Lambda)
    //////////////////////////////////////////////////////////////////////////
#define SERVICE_LEAVE( Type, Lambda )\
    SERVICE_PROVIDER_GET()->leaveService(Type::getStaticServiceID(), Lambda)
//////////////////////////////////////////////////////////////////////////
#define SERVICE_AVAILABLE( Type )\
    [](){ static bool available = SERVICE_GET(Type)->isAvailableService(); return available;}()
//////////////////////////////////////////////////////////////////////////