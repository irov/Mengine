#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
    class ServiceProviderInterface;

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
		virtual ServiceInterface * getService( const String & _name ) = 0;
		virtual bool registryService( const String & _name, ServiceInterface * _service ) = 0;
		virtual bool unregistryService( const String & _name ) = 0;

	public:
		virtual bool addServiceListener( const String & _name, ServiceListenerInterface * _serviceListener ) = 0;
		virtual bool removeServiceListener( const String & _name, ServiceListenerInterface * _serviceListener ) = 0;

	public:
		template<class T>
		T * getServiceT( const String & _name )
		{
            ServiceInterface * service = this->getService( _name );

            if( service == NULL )
            {
                return NULL;
            }

			return dynamic_cast<T*>(service);
		}        
	};

#   define SERVICE_DECLARE( NAME )\
    public:\
        inline static const char * getServiceName(){ return NAME; };\
    protected:

    template<class T>
    inline T * getService( ServiceProviderInterface * _serviceProvider )
    {
        static T * s_service = NULL;

        if( s_service == NULL )
        {
            const char * serviceName = T::getServiceName();
            s_service = _serviceProvider->getServiceT<T>(serviceName);
        }

        return s_service;
    }

#   define SERVICE_FACTORY( Name, Service, Implement )\
    extern "C"\
    {\
    bool create##Name( Service ** _service )\
    {\
    if( _service == NULL )\
    {\
    return false;\
    }\
    *_service = new Implement();\
    return true;\
    }\
    void destroy##Name( Service * _service )\
    {\
    if( _service )\
    {\
    delete static_cast<Implement *>(_service);\
    }\
    }\
    }\
    struct __mengine_dummy{}

#   define SERVICE_EXTERN( Name, Service )\
    extern "C"\
    {\
    extern bool create##Name( Service ** );\
        extern void destroy##Name( Service * );\
    }\
    struct __mengine_extern_dummy_##Name{}

#   define SERVICE_REGISTRY( Provider, Service )\
    (Provider->registryService(Service->getServiceName(), Service))
}