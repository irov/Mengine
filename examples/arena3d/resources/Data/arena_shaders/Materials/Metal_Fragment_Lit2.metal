#include <metal_stdlib>
using namespace metal;
struct FragmentIn { float4 position [[position]]; half4 color; half3 lighting; float2 uv0; };
fragment half4 fragmentShader(FragmentIn in [[stage_in]], texture2d<half> texture [[texture(0)]], sampler textureSampler [[sampler(0)]]) { half4 albedo = in.color * texture.sample(textureSampler, in.uv0); half3 lighting = max(0.5h, floor(in.lighting * 2.0h + 0.5h) / 2.0h); return half4(albedo.rgb * lighting, albedo.a); }
