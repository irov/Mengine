#include <metal_stdlib>
using namespace metal;

struct VertexIn { float3 position [[attribute(0)]]; float3 normal [[attribute(1)]]; float4 tangent [[attribute(2)]]; float4 color [[attribute(3)]]; float2 uv0 [[attribute(4)]]; float2 uv1 [[attribute(5)]]; };
struct VertexOut { float4 position [[position]]; half4 color; float2 uv0; };
struct CameraUniforms { float4x4 wvp; };
struct MeshUniforms { float4x4 world; float4 ambient; float4 directional; float4 directionalColor; float4 pointPosition[4]; float4 pointColor[4]; };

vertex VertexOut vertexShader(VertexIn in [[stage_in]], constant CameraUniforms & camera [[buffer(1)]], constant MeshUniforms & mesh [[buffer(2)]])
{
    VertexOut out;
    out.position = camera.wvp * float4(in.position, 1.0);
    float3 worldPosition = (mesh.world * float4(in.position, 1.0)).xyz;
    float3 worldNormal = normalize((mesh.world * float4(in.normal, 0.0)).xyz);
    float3 lighting = mesh.ambient.xyz;
    if (mesh.directional.w > 0.5) lighting += mesh.directionalColor.xyz * (mesh.directionalColor.w * max(0.0, dot(worldNormal, -mesh.directional.xyz)));
    for (uint index = 0; index != 4; ++index) {
        float invRadius = mesh.pointPosition[index].w;
        if (invRadius <= 0.0) continue;
        float3 toLight = mesh.pointPosition[index].xyz - worldPosition;
        float distance = length(toLight);
        float attenuation = max(0.0, 1.0 - distance * invRadius);
        attenuation *= attenuation;
        lighting += mesh.pointColor[index].xyz * (mesh.pointColor[index].w * max(0.0, dot(worldNormal, toLight / max(distance, 0.0001))) * attenuation);
    }
    out.color = half4(half3(in.color.rgb * lighting), half(in.color.a));
    out.uv0 = in.uv0;
    return out;
}
