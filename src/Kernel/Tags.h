#pragma once

#include "Kernel/ConstString.h"
#include "Kernel/VectorConstString.h"

#include "Config/Vector.h"

namespace Mengine
{
    class Tags
    {
    public:
        Tags();
        ~Tags();

    public:
        void addTag( const ConstString & _tag );
        void removeTag( const ConstString & _tag );
        bool hasTag( const ConstString & _tag ) const;
        bool hasTags( const Tags & _tag ) const;

        bool empty() const;
        void clear();

    public:
        const VectorConstString & getValues() const;

    protected:
        VectorConstString m_values;
    };
}
