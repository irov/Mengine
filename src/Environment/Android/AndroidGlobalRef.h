#pragma once

#include "Interface/DocumentInterface.h"

#include "Environment/Android/AndroidIncluder.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidGlobalRef
        : public Factorable
    {
    public:
        AndroidGlobalRef( JNIEnv * _jenv, jobject _value );
        ~AndroidGlobalRef() override;

    public:
        jobject getValue() const;

    protected:
        jobject m_value;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidGlobalRef> AndroidGlobalRefPtr;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        AndroidGlobalRefPtr makeAndroidGlobalRef( JNIEnv * _jenv, jobject _value, const DocumentInterfacePtr & _doc );
        //////////////////////////////////////////////////////////////////////////
    }
}
