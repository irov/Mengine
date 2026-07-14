#include <metal_stdlib>
using namespace metal;
struct FragmentIn { float4 position [[position]]; half4 color; float2 uv0; };
struct PostUniforms { float4 exposureVignetteChromatic; float4 hitFlash; float4 screenInfo; };
fragment half4 fragmentShader(FragmentIn in [[stage_in]], texture2d<half> texture [[texture(0)]], sampler textureSampler [[sampler(0)]], constant PostUniforms & post [[buffer(1)]])
{
    float2 direction = in.uv0 - post.screenInfo.xy;
    float2 chroma = direction * post.exposureVignetteChromatic.w * post.screenInfo.z;
    half3 color = half3(texture.sample(textureSampler, in.uv0 + chroma).r, texture.sample(textureSampler, in.uv0).g, texture.sample(textureSampler, in.uv0 - chroma).b);
    color *= half(post.exposureVignetteChromatic.x);
    color /= 1.0h + color;
    float distanceFromCenter = length(direction) * 1.4142136;
    float softness = max(post.exposureVignetteChromatic.z, 0.0001);
    float vignette = clamp((distanceFromCenter - (1.0 - softness)) / softness, 0.0, 1.0);
    color *= half(1.0 - vignette * post.exposureVignetteChromatic.y);
    color = saturate(color + half3(post.hitFlash.xyz * post.hitFlash.w));
    return half4(color, texture.sample(textureSampler, in.uv0).a) * in.color;
}
