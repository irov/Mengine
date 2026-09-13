cbuffer CameraConstants : register( b0 )
{
    float4x4 ModelViewMatrix;
};
//////////////////////////////////////////////////////////////////////////
cbuffer MeshConstants : register( b4 )
{
    float4x4 WorldMatrix;
    float4 Ambient;
    float4 Directional;
    float4 DirectionalColor;
    float4 PointPosition[4];
    float4 PointColor[4];
    float4 MeshColor;
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
    float4 color : COLOR0;
    float2 uv0 : TEXCOORD0;
};
//////////////////////////////////////////////////////////////////////////
void main( in VSInput _input, out VSOutput _output )
{
    _output.position = mul( _input.position, ModelViewMatrix );
    _output.color = _input.color * MeshColor;
    _output.uv0 = _input.uv0;
}
