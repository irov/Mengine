#include <metal_stdlib>
using namespace metal;
struct FragmentIn { float4 position [[position]]; half4 color; float2 uv0; };
fragment half4 fragmentShader(FragmentIn in [[stage_in]], texture2d<half> texture [[texture(0)]], sampler textureSampler [[sampler(0)]]) { half4 value = in.color * texture.sample(textureSampler, in.uv0); value.rgb = floor(value.rgb * 2.0h + 0.5h) / 2.0h; return value; }
