#include "AndroidLanguageSystem.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"
#include "Environment/Android/AndroidApplicationHelper.h"

#include "Kernel/AssertionMemoryPanic.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( LanguageSystem, Mengine::AndroidLanguageSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidLanguageSystem::AndroidLanguageSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidLanguageSystem::~AndroidLanguageSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidLanguageSystem::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidLanguageSystem::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidLanguageSystem::getUserLanguage( Char * const _userLanguage, size_t _capacity ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jlanguage = (jstring)Helper::AndroidCallObjectApplicationMethod( jenv, "getDeviceLanguage", "()Ljava/lang/String;" );

        Helper::AndroidCopyStringFromJString( jenv, jlanguage, _userLanguage, _capacity );

        Mengine_JNI_DeleteLocalRef( jenv, jlanguage );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
