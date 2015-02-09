#	pragma once

#	include "Config/Typedef.h"

#	include "Core/Exception.h"

#   ifdef _DEBUG
#	   include <typeinfo>
#	endif

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
    class ServiceProviderInterface;
	//////////////////////////////////////////////////////////////////////////
	class ServiceInterface
	{
	public:
		ServiceInterface(){};
		virtual ~ServiceInterface(){};

    public:
        virtual void setServiceProvider( ServiceProviderInterface * _serviceProvider ) = 0;
        virtual ServiceProviderInterface * getServiceProvider() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	class ServiceListenerInterface
	{
	public:
		virtual bool onRegistryService( ServiceProviderInterface * _serviceProvider, ServiceInterface * _service ) = 0;
        virtual void onUnregistryService( ServiceProviderInterface * _serviceProvider, ServiceInterface * _service ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	class ServiceProviderInterface
	{
	public:
		ServiceProviderInterface(){};
		virtual ~ServiceProviderInterface(){};

	public:
		virtual bool existService( const char * _name ) const = 0;

	public:
		virtual ServiceInterface * getService( const char * _name ) const = 0;

    public:
		virtual bool registryService( const char * _name, ServiceInterface * _service ) = 0;
		virtual bool unregistryService( const char * _name ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
		//////////////////////////////////////////////////////////////////////////
        template<class T>
#   ifdef _DEBUG
        inline T * getService( ServiceProviderInterface * _serviceProvider, const char * _file, uint32_t _line )
#	else
		inline T * getService( ServiceProviderInterface * _serviceProvider )
#	endif
        {
            static T * s_service = nullptr;

            if( s_service == nullptr )
            {
                const char * serviceName = T::getServiceName();

                ServiceInterface * service = _serviceProvider->getService( serviceName );

#   ifdef _DEBUG
				if( service == nullptr )
				{
					MENGINE_THROW_EXCEPTION_FL(_file, _line)("Service %s not found"
						, serviceName
						);
				}

                if( dynamic_cast<T *>(service) == nullptr )
                {
					MENGINE_THROW_EXCEPTION_FL(_file, _line)("Service %s invalid cast to %s"
						, serviceName
						, typeid(T).name()
						);
                }
#   endif

                s_service = static_cast<T *>(service);
            }

            return s_service;
        }
		//////////////////////////////////////////////////////////////////////////
		template<class T>
		inline bool existService( ServiceProviderInterface * _serviceProvider )
		{
			static bool s_initialize = false;
			static bool s_exist = false;

			if( s_initialize == false )
			{
				s_initialize = true;

				const char * serviceName = T::getServiceName();

				s_exist = _serviceProvider->existService( serviceName );
			}

			return s_exist;
		}
    }
	//////////////////////////////////////////////////////////////////////////
#	ifdef _DEBUG
#	define SERVICE_GET( serviceProvider, Type )\
	(Menge::Helper::getService<Type>(serviceProvider, __FILE__, __LINE__))
#	else
#	define SERVICE_GET( serviceProvider, Type )\
	(Menge::Helper::getService<Type>(serviceProvider))
#	endif

#	define SERVICE_EXIST( serviceProvider, Type )\
	(Menge::Helper::existService<Type>(serviceProvider))

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
    delete _service;\
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

#   define SERVICE_UNREGISTRY( Provider, Service )\
	Provider->unregistryService( Service->getServiceName() )

#   define SERVICE_CREATE( Name, Service )\
    SERVICE_NAME_CREATE(Name)( Service )

#   define SERVICE_DESTROY( Name, Service )\
	SERVICE_NAME_DESTROY(Name)( Service )

#	define SERVICE_THREAD_SAFE_METHOD( Type ) Type
}