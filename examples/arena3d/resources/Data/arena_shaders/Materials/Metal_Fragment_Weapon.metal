#include <metal_stdlib>
using namespace metal;
//////////////////////////////////////////////////////////////////////////
struct FragmentIn
{
    float4 position [[position]];
    half4 color;
    float3 normal;
    float3 positionModel;
    float2 uv0;
};
//////////////////////////////////////////////////////////////////////////
fragment half4 fragmentShader( FragmentIn _input [[stage_in]], texture2d<half> _texture [[texture( 0 )]], sampler _textureSampler [[sampler( 0 )]] )
{
    half4 albedo = _input.color * _texture.sample( _textureSampler, _input.uv0 );
    if( albedo.a < 0.5h )
    {
        discard_fragment();
    }
    float3 normal = normalize( _input.normal );
    float3 lightDirection = float3( -0.45, 0.8, -0.6 );
    float3 light = normalize( lightDirection );
    float normalLight = dot( normal, light );
    float diffuse = max( 0.0, normalLight );
    float3 eyePosition = float3( 0.0, 0.0, -1.0 );
    float3 eyeDirection = eyePosition - _input.positionModel;
    float3 eye = normalize( eyeDirection );
    float3 halfDirection = light + eye;
    float3 halfVector = normalize( halfDirection );
    float normalHalf = dot( normal, halfVector );
    float highlight = max( 0.0, normalHalf );
    float specular = pow( highlight, 28.0 );
    float diffuseLight = 0.48 + diffuse * 0.7;
    half3 specularColor = half3( 0.16, 0.19, 0.23 );
    half3 litColor = albedo.rgb * half( diffuseLight ) + specularColor * half( specular );
    half4 color = half4( litColor, 1.0h );
    return color;
}
