#pragma once

#include "Interface/ServiceProviderInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ServiceInterface
    {
    public:
        ServiceInterface() {};
        virtual ~ServiceInterface() {};

    public:
        virtual const Char * getServiceID() const = 0;

    protected:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    protected:
        virtual void stopService() = 0;

    protected:
        virtual bool isInitialize() const = 0;

    public:
        virtual void destroy() = 0;

    public:
        friend class ServiceProvider;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        inline T * getService( const Char * _file, uint32_t _line )
        {
            (void)_file;
            (void)_line;

            static T * s_service = nullptr;

            if( s_service == nullptr )
            {
                const Char * serviceName = T::getStaticServiceID();

                ServiceProviderInterface * serviceProvider = SERVICE_PROVIDER_GET();

#ifdef _DEBUG
                if( serviceProvider == nullptr )
                {
                    MENGINE_THROW_EXCEPTION_FL( _file, _line )("Service %s invalid get provider"
                        , serviceName
                        );
                }
#endif

                ServiceInterface * service = serviceProvider->getService( serviceName );

#ifdef _DEBUG
                if( service == nullptr )
                {
                    MENGINE_THROW_EXCEPTION_FL( _file, _line )("Service %s not found"
                        , serviceName
                        );
                }

                if( dynamic_cast<T *>(service) == nullptr )
                {
                    MENGINE_THROW_EXCEPTION_FL( _file, _line )("Service %s invalid cast to %s"
                        , serviceName
                        , typeid(T).name()
                        );
                }
#endif

                s_service = static_cast<T *>(service);
            }

            return s_service;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        inline bool existService()
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
//////////////////////////////////////////////////////////////////////////
#define SERVICE_GET( Type )\
	(Mengine::Helper::getService<Type>(__FILE__, __LINE__))
//////////////////////////////////////////////////////////////////////////
#define SERVICE_EXIST( Type )\
	(Mengine::Helper::existService<Type>())
//////////////////////////////////////////////////////////////////////////
#define SERVICE_NAME_CREATE(Name)\
	__createMengineService##Name
//////////////////////////////////////////////////////////////////////////
#define SERVICE_DECLARE( ID )\
    public:\
        inline static const Char * getStaticServiceID(){ return ID; };\
		inline const Char * getServiceID() const override { return ID; };\
    protected:
//////////////////////////////////////////////////////////////////////////
#define SERVICE_FACTORY( Name, Implement )\
    bool SERVICE_NAME_CREATE(Name)(Mengine::ServiceInterface**_service){\
    if(_service==nullptr){return false;}\
	try{*_service=new Implement();}catch(...){return false;}\
    return true;}\
	struct __mengine_dummy_factory##Name{}
//////////////////////////////////////////////////////////////////////////
#define SERVICE_EXTERN(Name)\
	extern bool SERVICE_NAME_CREATE( Name )(Mengine::ServiceInterface**);
//////////////////////////////////////////////////////////////////////////
#define SERVICE_CREATE( Name )\
	SERVICE_PROVIDER_GET()->initializeService(&SERVICE_NAME_CREATE(Name))
//////////////////////////////////////////////////////////////////////////
#define SERVICE_GENERATE( Name, Type )\
	SERVICE_PROVIDER_GET()->generateServiceT<Type>(&SERVICE_NAME_CREATE(Name), __FILE__, __LINE__)
//////////////////////////////////////////////////////////////////////////
#define SERVICE_FINALIZE( Type )\
	SERVICE_PROVIDER_GET()->finalizeServiceT<Type>()
//////////////////////////////////////////////////////////////////////////
#define SERVICE_DESTROY( Type )\
	SERVICE_PROVIDER_GET()->destroyServiceT<Type>()
}