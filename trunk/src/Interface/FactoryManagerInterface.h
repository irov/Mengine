#   pragma once

#   include "Core/ConstString.h"

namespace Menge
{
    class Factory;
    class Factorable;

    class VisitorFactoryManager
    {
    public:
        virtual void visit( const ConstString & _type, Factory * _factory ) = 0;
    };

    class FactoryManagerInterface
    {
    public:
        virtual void registerFactory( const ConstString & _type, Factory * _factory ) = 0;
        virtual void unregisterFactory( const ConstString & _type ) = 0;

    public:
        virtual bool hasFactory( const ConstString & _type ) const = 0;

    public:
        virtual Factorable * createObject( const ConstString & _type ) = 0;

    public:
        virtual void visitFactories( VisitorFactoryManager * _visit ) = 0;
    };
}
