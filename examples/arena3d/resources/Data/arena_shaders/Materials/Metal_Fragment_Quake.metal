#include <metal_stdlib>
using namespace metal;
//////////////////////////////////////////////////////////////////////////
struct FragmentIn
{
    float4 position [[position]];
    float2 uv0;
    float2 uv1;
};
//////////////////////////////////////////////////////////////////////////
fragment half4 fragmentShader( FragmentIn _input [[stage_in]], texture2d<half> _texture [[texture( 0 )]], sampler _textureSampler [[sampler( 0 )]] )
{
    uint width = _texture.get_width();
    uint height = _texture.get_height();
    float2 size = float2( width, height );
    float2 repeatedUV = fract( _input.uv0 );
    float2 uv = ( repeatedUV * 510.0 + 1.0 ) / size;
    half3 albedo = _texture.sample( _textureSampler, uv ).rgb;
    half3 lightmap = _texture.sample( _textureSampler, _input.uv1 ).rgb;
    half3 litColor = albedo * lightmap * 2.0h;
    half4 color = half4( litColor, 1.0h );
    return color;
}
