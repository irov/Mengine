#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
    class ServiceProviderInterface;

	class ServiceNotFoundException
	{
	};

	template<class T>
	class ServiceNotFoundExceptionT
		: public ServiceNotFoundException
	{
	};

	class ServiceInterface
	{
    public:
        virtual void setServiceProvider( ServiceProviderInterface * _serviceProvider ) = 0;
        virtual ServiceProviderInterface * getServiceProvider() const = 0;
	};

	class ServiceListenerInterface
	{
	public:
		virtual bool onRegistryService( ServiceProviderInterface * _serviceProvider, ServiceInterface * _service ) = 0;
        virtual void onUnregistryService( ServiceProviderInterface * _serviceProvider, ServiceInterface * _service ) = 0;
	};

	class ServiceProviderInterface
	{
	public:
		virtual ServiceInterface * getService( const char * _name ) const = 0;

    public:
		virtual bool registryService( const char * _name, ServiceInterface * _service ) = 0;
		virtual bool unregistryService( const char * _name ) = 0;
	};

    namespace Helper
    {
        template<class T>
        inline T * getService( ServiceProviderInterface * _serviceProvider )
        {
            static T * s_service = nullptr;

            if( s_service == nullptr )
            {
                const char * serviceName = T::getServiceName();

                ServiceInterface * service = _serviceProvider->getService( serviceName );

#   ifdef _DEBUG
                if( dynamic_cast<T*>(service) == nullptr )
                {
					throw ServiceNotFoundExceptionT<T>();
                }
#   endif

                s_service = static_cast<T *>(service);
            }

            return s_service;
        }
    }

#	define SERVICE_NAME_CREATE( Name )\
	serviceCreate##Name

#	define SERVICE_NAME_DESTROY( Name )\
	serviceDestroy##Name


#   define SERVICE_DECLARE( Name )\
    public:\
        inline static const char * getServiceName(){ return Name; };\
    protected:

#   define SERVICE_FACTORY( Name, Service, Implement )\
    bool SERVICE_NAME_CREATE(Name)( Service ** _service )\
    {\
    if( _service == nullptr )\
    {\
    return false;\
    }\
    *_service = new Implement();\
    return true;\
    }\
    void SERVICE_NAME_DESTROY(Name)( Service * _service )\
    {\
    delete static_cast<Implement *>(_service);\
    }\
	struct __mengine_dummy_factory##Name{}

#   define SERVICE_EXTERN( Name, Service )\
    extern bool SERVICE_NAME_CREATE(Name)( Service ** );\
    extern void SERVICE_NAME_DESTROY(Name)( Service * );\
    struct __mengine_dummy_extern_##Name{}

#   define SERVICE_DUMMY( Name, Service )\
    bool SERVICE_NAME_CREATE(Name)( Service ** ){return false;};\
    void SERVICE_NAME_DESTROY(Name)( Service * ){};\
    struct __mengine_dummy_extern_##Name{}

#   define SERVICE_REGISTRY( Provider, Service )\
    Provider->registryService( Service->getServiceName(), Service )

#   define SERVICE_CREATE( Name, Service )\
    SERVICE_NAME_CREATE(Name)( Service )

#   define SERVICE_DESTROY( Name, Service )\
	SERVICE_NAME_DESTROY(Name)( Service )
}