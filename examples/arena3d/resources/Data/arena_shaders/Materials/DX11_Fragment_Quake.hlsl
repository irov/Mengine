Texture2D texture0 : register( t0 );
SamplerState sampler0 : register( s0 );
struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};
//////////////////////////////////////////////////////////////////////////
float4 main( PSInput _input ) : SV_TARGET
{
    uint width;
    uint height;
    texture0.GetDimensions( width, height );
    float2 size = float2( width, height );
    float2 repeatedUV = frac( _input.uv0 );
    float2 uv = ( repeatedUV * 510.0 + 1.0 ) / size;
    float3 albedo = texture0.Sample( sampler0, uv ).rgb;
    float3 lightmap = texture0.Sample( sampler0, _input.uv1 ).rgb;
    float3 litColor = albedo * lightmap * 2.0;
    float4 color = float4( litColor, 1.0 );
    return color;
}
