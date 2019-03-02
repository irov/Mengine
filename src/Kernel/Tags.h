#pragma once

#include "Kernel/ConstString.h"
#include "Kernel/VectorConstString.h"

#include "Config/Vector.h"
#include "Config/String.h"

namespace Mengine
{
    class Tags
    {
    public:
        Tags();
        ~Tags();

    public:
        void addTag( const ConstString & _tag );
        bool hasTag( const ConstString & _tag ) const;
        bool inTags( const Tags & _tag ) const;

        bool empty() const;
        void clear();

    public:
        const VectorConstString & getTags() const;

    public:
        void swap( Tags & _tags );

    public:
        String to_str() const;

    protected:
        VectorConstString m_tags;
    };
}

namespace std
{
    template<> inline void swap( Mengine::Tags & _left, Mengine::Tags & _right )
    {
        _left.swap( _right );
    }
}
