#include "ImGUIMetalTexture.h"

//////////////////////////////////////////////////////////////////////////
@implementation MengineImGUIMetalTexture

- (instancetype)initWithTexture:(id<MTLTexture>)_texture
{
    self = [super init];

    if( self == nil )
    {
        return nil;
    }

    self.texture = _texture;

    return self;
}

@end
