#include <metal_stdlib>
using namespace metal;
struct VertexIn { float3 position [[attribute(0)]]; float3 normal [[attribute(1)]]; float4 tangent [[attribute(2)]]; float4 color [[attribute(3)]]; float2 uv0 [[attribute(4)]]; float2 uv1 [[attribute(5)]]; };
struct VertexOut { float4 position [[position]]; half4 color; float2 uv0; };
struct CameraUniforms { float4x4 wvp; };
vertex VertexOut vertexShader(VertexIn in [[stage_in]], constant CameraUniforms & camera [[buffer(1)]]) { VertexOut out; out.position = camera.wvp * float4(in.position, 1.0); out.color = half4(in.color); out.uv0 = in.uv0; return out; }
