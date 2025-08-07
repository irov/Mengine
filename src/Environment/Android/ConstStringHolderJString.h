#pragma once

#include "Environment/Android/AndroidEnv.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/ConstStringHolder.h"
#include "Kernel/IntrusiveLinked.h"

namespace Mengine
{
    class ConstStringHolderJString
        : public ConstStringHolder
        , public IntrusiveLinked<ConstStringHolderJString>
    {
    public:
        ConstStringHolderJString();
        ~ConstStringHolderJString() override;

    public:
        void setJString( MengineJNIEnvThread * _jenv, jstring _value );
        void removeJString( MengineJNIEnvThread * _jenv );

    protected:
        jstring m_value;
    };
}
