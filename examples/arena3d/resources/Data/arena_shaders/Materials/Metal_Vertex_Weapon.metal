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
    float3 normal;
    float3 positionModel;
    float2 uv0;
};
//////////////////////////////////////////////////////////////////////////
struct CameraUniforms
{
    float4x4 wvp;
};
//////////////////////////////////////////////////////////////////////////
vertex VertexOut vertexShader( VertexIn _input [[stage_in]], constant CameraUniforms & _camera [[buffer( 1 )]] )
{
    VertexOut output;
    output.position = _camera.wvp * float4( _input.position, 1.0 );
    output.color = half4( _input.color );
    output.normal = _input.normal;
    output.positionModel = _input.position;
    output.uv0 = _input.uv0;
    return output;
}
