#pragma once

#include "Interface/ArgumentsInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/VectorString.h"

namespace Mengine
{
    class StringArguments
        : public ArgumentsInterface
        , public Factorable
    {
    public:
        StringArguments();
        ~StringArguments() override;

    protected:
        void addArgument( const Char * _argument ) override;
        void addArguments( const Char * _arguments ) override;
        const Char * getArgument( uint32_t _index ) const override;
        uint32_t getArgumentCount() const override;

    protected:
        VectorString m_arguments;
    };
}