#include "AndroidAttributionProvider.h"

#include "Interface/AttributionServiceInterface.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"
#include "Environment/Android/AndroidFragmentHelper.h"

#include "Kernel/ThreadHelper.h"

//////////////////////////////////////////////////////////////////////////
extern "C" JNIEXPORT void JNICALL Java_org_Mengine_Base_MengineFragmentAttribution_nativeClearProject( JNIEnv * _env, jclass _class )
{
    MENGINE_UNUSED( _env );
    MENGINE_UNUSED( _class );

    Mengine::Helper::dispatchMainThreadEvent( []()
    {
        if( SERVICE_IS_INITIALIZE( Mengine::AttributionServiceInterface ) == false )
        {
            return;
        }

        ATTRIBUTION_SERVICE()
            ->clearAttributions();
    } );
}
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidAttributionProvider::AndroidAttributionProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidAttributionProvider::~AndroidAttributionProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidAttributionProvider::onAttributionChanged( const ConstString & _name, EAttributionType _type, const ParamVariant & _value )
    {
        MENGINE_UNUSED( _type );

        JNIEnv * env = Mengine_JNI_GetEnv();
        if( env == nullptr )
        {
            return;
        }

        jstring name = Helper::AndroidMakeJObjectString( env, _name );
        jobject value = Helper::AndroidMakeJObjectParamVariant( env, _value );

        Helper::AndroidCallVoidFragmentMethod( env, "MengineFragmentAttribution", "setAttribution", "(Ljava/lang/String;Ljava/lang/Object;)V", name, value );

        Mengine_JNI_DeleteLocalRef( env, value );
        Mengine_JNI_DeleteLocalRef( env, name );
    }
}
//////////////////////////////////////////////////////////////////////////
