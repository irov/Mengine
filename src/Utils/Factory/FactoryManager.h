#	pragma once

#   include "Interface/ServiceInterface.h"

#	include "Core/ConstString.h"
#	include "Core/BinaryVector.h"

namespace Menge
{
    class Factory;

    class VisitorFactoryManager
    {
    public:
        virtual void visit( const ConstString & _type, Factory * _factory ) = 0;
    };
    
	class FactoryManager
	{
	public:
		FactoryManager();
		~FactoryManager();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider );

	public:
		void registerFactory( const ConstString & _type, Factory * _factory );
		void unregisterFactory( const ConstString & _type );

	public:
		bool hasFactory( const ConstString & _type ) const;

	public:
		Factorable * createObject( const ConstString & _type );

        template<class T>
        T * createObjectT( const ConstString & _type )
        {
            Factorable * factorable = this->createObject( _type );

#   ifdef _DEBUG
            if( dynamic_cast<T*>(factorable) == nullptr )
            {
                return nullptr;
            }
#   endif

            T * t = static_cast<T*>(factorable);			

            return t;
        }

	public:
		void visitFactories( VisitorFactoryManager * _visit );

	protected:
        ServiceProviderInterface * m_serviceProvider;

		typedef BinaryVector<ConstString, Factory *> TMapFactory;
		TMapFactory m_factories;
	};
}
