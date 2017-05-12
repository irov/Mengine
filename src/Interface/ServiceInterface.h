#	pragma once

#	include "Config/Typedef.h"
#	include "Config/Char.h"

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

	public:
		virtual const Char * getServiceID() const = 0;

	protected:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

	protected:
		virtual bool isInitialize() const = 0;

	public:
		virtual void destroy() = 0;

	public:
		friend class ServiceProvider;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef bool (*TServiceProviderGenerator)(Menge::ServiceInterface**);
	//////////////////////////////////////////////////////////////////////////
	class ServiceProviderInterface
	{
	public:
		ServiceProviderInterface(){};
		virtual ~ServiceProviderInterface(){};

	public:
		virtual bool existService( const Char * _name ) const = 0;

	public:
		virtual ServiceInterface * getService( const Char * _name ) const = 0;

    public:
        virtual ServiceInterface * generateService( TServiceProviderGenerator _generator ) = 0;

        template<class T>
        T * generateServiceT( TServiceProviderGenerator _generator, const Char * _file, uint32_t _line )
        {
            ServiceInterface * service = this->generateService( _generator );

#   ifdef _DEBUG
            const Char * serviceName = T::getStaticServiceID();

            if( service == nullptr )
            {
                MENGINE_THROW_EXCEPTION_FL( _file, _line )("Generate service %s not found"
                    , serviceName
                    );
            }

            if( dynamic_cast<T *>(service) == nullptr )
            {
                MENGINE_THROW_EXCEPTION_FL( _file, _line )("Genereate service %s invalid cast to '%s'"
                    , serviceName
                    , typeid(T).name()
                    );

                throw;
            }
#	else
			(void)_file;
			(void)_line;
#   endif
            T * t = static_cast<T *>(service);

            return t;
        }

    public:
		virtual bool initializeService( TServiceProviderGenerator _generator ) = 0;
		virtual bool finalizeService( const Char * _name ) = 0;
		virtual bool destroyService( const Char * _name ) = 0;

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
    namespace Helper
    {
		//////////////////////////////////////////////////////////////////////////
        template<class T>
		inline T * getService( ServiceProviderInterface * _serviceProvider, const Char * _file, uint32_t _line )
        {
            (void)_file;
            (void)_line;

            static T * s_service = nullptr;

            if( s_service == nullptr )
            {
				const Char * serviceName = T::getStaticServiceID();

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

                    throw;
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

			if( s_exist == false )
			{				
				const Char * serviceName = T::getStaticServiceID();

				if( _serviceProvider->existService( serviceName ) == false )
				{
					s_exist = false;
					s_initialize = false;
				}
				else
				{
					s_exist = true;
					s_initialize = true;
				}
			}

			return s_exist;
		}
    }
    //////////////////////////////////////////////////////////////////////////
#	define SERVICE_GET( serviceProvider, Type )\
	(Menge::Helper::getService<Type>(serviceProvider, __FILE__, __LINE__))

#	define SERVICE_EXIST( serviceProvider, Type )\
	(Menge::Helper::existService<Type>(serviceProvider))

#	define SERVICE_NAME_CREATE(Name)\
	__createMengineService##Name

#	define SERVICE_PROVIDER_NAME_CREATE(Name)\
	__createMengineProvider##Name

#   define SERVICE_DECLARE( ID )\
    public:\
        inline static const Char * getStaticServiceID(){ return ID; };\
		inline const Char * getServiceID() const override { return ID; };\
    protected:

#   define SERVICE_FACTORY( Name, Implement )\
    bool SERVICE_NAME_CREATE(Name)(Menge::ServiceInterface**_service){\
    if(_service==nullptr){return false;}\
	try{*_service=new Implement();}catch(...){return false;}\
    return true;}\
	struct __mengine_dummy_factory##Name{}

#   define SERVICE_PROVIDER_FACTORY( Name, Implement )\
    bool SERVICE_PROVIDER_NAME_CREATE(Name)(Menge::ServiceProviderInterface**_serviceProvider){\
    if(_serviceProvider==nullptr){return false;}\
	try{*_serviceProvider=new Implement();}catch(...){return false;}\
    return true;}\
	struct __mengine_dummy_factory##Name{}

#	define SERVICE_PROVIDER_EXTERN(Name)\
	extern bool SERVICE_PROVIDER_NAME_CREATE( Name )(Menge::ServiceProviderInterface**);

#   define SERVICE_PROVIDER_CREATE( Name, Provider )\
	SERVICE_PROVIDER_NAME_CREATE(Name)(Provider)

#	define SERVICE_PROVIDER_FINALIZE( Provider )\
	Provider->destroy()

#	define SERVICE_EXTERN(Name)\
	extern bool SERVICE_NAME_CREATE( Name )(Menge::ServiceInterface**);

#   define SERVICE_CREATE( Provider, Name )\
	( Provider != nullptr ? Provider->initializeService(&SERVICE_NAME_CREATE(Name)) : false )

#   define SERVICE_GENERATE( Provider, Name, Type )\
	( Provider != nullptr ? Provider->generateServiceT<Type>(&SERVICE_NAME_CREATE(Name), __FILE__, __LINE__) : nullptr )

#   define SERVICE_FINALIZE( Provider, Type )\
	( Provider != nullptr ? Provider->finalizeServiceT<Type>() : false )

#   define SERVICE_DESTROY( Provider, Type )\
	( Provider != nullptr ? Provider->destroyServiceT<Type>() : false )
}