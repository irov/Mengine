cbuffer CameraConstants : register( b0 )
{
    float4x4 ModelViewMatrix;
};
//////////////////////////////////////////////////////////////////////////
struct VSInput
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 color : COLOR0;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};
//////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};
//////////////////////////////////////////////////////////////////////////
void main( in VSInput _input, out VSOutput _output )
{
    _output.position = mul( _input.position, ModelViewMatrix );
    _output.uv0 = _input.uv0;
    _output.uv1 = _input.uv1;
}
