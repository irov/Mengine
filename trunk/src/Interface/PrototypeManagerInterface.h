#   pragma once

#   include "Interface/ServiceInterface.h"

#   include "Core/ConstString.h"

namespace Menge
{
    class PrototypeGeneratorInterface
    {
    public:
        virtual Factorable * generate( const ConstString & _category, const ConstString & _prototype ) = 0;
        virtual size_t count() const = 0;

        virtual void destroy() = 0; 
    };

    class VisitorPrototypeGenerator
    {
    public:
        virtual void visit( const ConstString & _category, const ConstString & _type, PrototypeGeneratorInterface * _factory ) = 0;
    };

    class PrototypeServiceInterface
		: public ServiceInterface
    {
        SERVICE_DECLARE("PrototypeService")

    public:
        virtual bool addPrototype( const ConstString & _category, const ConstString & _prototype, PrototypeGeneratorInterface * _generator ) = 0;
        virtual bool hasPrototype( const ConstString & _category, const ConstString & _prototype, PrototypeGeneratorInterface ** _generator ) const = 0;

    public:
        virtual Factorable * generatePrototype( const ConstString & _category, const ConstString & _prototype ) = 0;

    public:
        template<class T>
        T * generatePrototypeT( const ConstString & _category, const ConstString & _prototype )
        {
            Factorable * prototype = this->generatePrototype( _category, _prototype );

#   ifdef _DEBUG
            if( dynamic_cast<T *>(prototype) == nullptr )
            {
                return nullptr;
            }
#   endif

            T * t = static_cast<T *>(prototype);

            return t;
        }

    public:
        virtual void visitGenerators( VisitorPrototypeGenerator * _visitor ) const = 0;
    };
}

#   define PROTOTYPE_SERVICE( serviceProvider )\
    (Menge::Helper::getService<Menge::PrototypeServiceInterface>(serviceProvider))

