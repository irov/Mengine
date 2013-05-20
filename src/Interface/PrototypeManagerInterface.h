#   pragma once

#   include "Interface/ServiceInterface.h"

#   include "Core/ConstString.h"

namespace Menge
{
    class PrototypeInterface
    {
    };

    class PrototypeGeneratorInterface
    {
    public:
        virtual PrototypeInterface * generate( const ConstString & _category, const ConstString & _prototype ) = 0;
        virtual void destroy() = 0; 
    };

    class PrototypeServiceInterface
		: public ServiceInterface
    {
        SERVICE_DECLARE("PrototypeService")

    public:
        virtual bool addPrototype( const ConstString & _category, const ConstString & _prototype, PrototypeGeneratorInterface * _generator ) = 0;
        virtual bool hasPrototype( const ConstString & _category, const ConstString & _prototype, PrototypeGeneratorInterface ** _generator ) const = 0;

    public:
        virtual PrototypeInterface * generatePrototype( const ConstString & _category, const ConstString & _prototype ) = 0;

    public:
        template<class T>
        T * generatePrototypeT( const ConstString & _category, const ConstString & _prototype )
        {
            PrototypeInterface * prototype = this->generatePrototype( _category, _prototype );

            T * t = static_cast<T *>(prototype);

            return t;
        }
    };
}

#   define PROTOTYPE_SERVICE( serviceProvider )\
    (Menge::Helper::getService<Menge::PrototypeServiceInterface>(serviceProvider))

