Texture2D texture0 : register( t0 );
SamplerState sampler0 : register( s0 );
struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
    float3 normal : TEXCOORD1;
    float3 positionModel : TEXCOORD2;
    float2 uv0 : TEXCOORD0;
};
//////////////////////////////////////////////////////////////////////////
float4 main( PSInput _input ) : SV_TARGET
{
    float4 albedo = _input.color * texture0.Sample( sampler0, _input.uv0 );
    if( albedo.a < 0.5 )
    {
        discard;
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
    float3 specularColor = float3( 0.16, 0.19, 0.23 );
    float3 litColor = albedo.rgb * float( diffuseLight ) + specularColor * float( specular );
    float4 color = float4( litColor, 1.0 );
    return color;
}
