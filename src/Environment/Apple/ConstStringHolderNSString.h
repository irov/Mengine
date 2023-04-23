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
        void setNSString( NSString * _value );

    protected:
        NSString * m_value;
    };
}