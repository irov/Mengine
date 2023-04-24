#pragma once

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/ConstStringHolder.h"
#include "Kernel/IntrusiveLinked.h"

#import <Foundation/Foundation.h>

namespace Mengine
{
    class ConstStringHolderNSString
        : public ConstStringHolder
        , public IntrusiveLinked<ConstStringHolderNSString>
    {
    public:
        ConstStringHolderNSString();
        ~ConstStringHolderNSString();

    public:
        void setNSString( const NSString * _value );

    protected:
        const NSString * m_value;
    };
}
