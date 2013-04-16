#	pragma once

#   include "Interface/FactoryManagerInterface.h"

#	include "Factory.h"

#	include "Core/ConstString.h"

#	include <map>

namespace Menge
{
    class Factory;
    class ServiceProviderInterface;

	class FactoryManager
        : virtual public FactoryManagerInterface
	{
	public:
		FactoryManager();
		~FactoryManager();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider );

	public:
		void registerFactory( const ConstString & _type, Factory * _factory ) override;
		void unregisterFactory( const ConstString & _type ) override;

	public:
		bool hasFactory( const ConstString & _type ) const override;

	public:
		Factorable * createObject( const ConstString & _type ) override;

        template<class T>
        T * createObjectT( const ConstString & _type )
        {
            Factorable * factorable = this->createObject( _type );

            if( dynamic_cast<T*>(factorable) == NULL )
            {
                return NULL;
            }

            T * t = static_cast<T*>(factorable);			

            return t;
        }

	public:
		void visitFactories( VisitorFactoryManager * _visit ) override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		typedef std::map<ConstString, Factory *> TMapFactory;
		TMapFactory m_factories;
	};
}
