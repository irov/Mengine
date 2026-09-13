#include <metal_stdlib>
using namespace metal;
//////////////////////////////////////////////////////////////////////////
struct VertexIn
{
    float3 position [[attribute( 0 )]];
    float3 normal [[attribute( 1 )]];
    float4 tangent [[attribute( 2 )]];
    float4 color [[attribute( 3 )]];
    float2 uv0 [[attribute( 4 )]];
    float2 uv1 [[attribute( 5 )]];
};
//////////////////////////////////////////////////////////////////////////
struct VertexOut
{
    float4 position [[position]];
    half4 color;
    float2 uv0;
};
//////////////////////////////////////////////////////////////////////////
struct CameraUniforms
{
    float4x4 wvp;
};
//////////////////////////////////////////////////////////////////////////
struct MeshUniforms
{
    float4x4 world;
    float4 ambient;
    float4 directional;
    float4 directionalColor;
    float4 pointPosition[4];
    float4 pointColor[4];
    float4 color;
};
//////////////////////////////////////////////////////////////////////////
vertex VertexOut vertexShader( VertexIn _input [[stage_in]], constant CameraUniforms & _camera [[buffer( 1 )]], constant MeshUniforms & _mesh [[buffer( 2 )]] )
{
    float4 position = float4( _input.position, 1.0 );
    float4 color = _input.color * _mesh.color;
    VertexOut output;
    output.position = _camera.wvp * position;
    output.color = half4( color );
    output.uv0 = _input.uv0;
    return output;
}
