#include "iOSAttributionProvider.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleString.h"
#import "Environment/iOS/iOSAttribution.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    iOSAttributionProvider::iOSAttributionProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSAttributionProvider::~iOSAttributionProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSAttributionProvider::onAttributionChanged( const ConstString & _name, EAttributionType _type, const ParamVariant & _value )
    {
        MENGINE_UNUSED( _type );

        NSString * name = [AppleString NSStringFromConstString:_name];
        id value = [AppleDetail getNSObjectFromParamVariant:_value];

        [iOSAttribution setAttribution:name value:value];
    }
    //////////////////////////////////////////////////////////////////////////
}
