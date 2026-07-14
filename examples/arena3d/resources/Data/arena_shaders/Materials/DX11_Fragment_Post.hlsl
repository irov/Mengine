Texture2D texture0 : register(t0); SamplerState sampler0 : register(s0);
cbuffer PostConstants : register(b0) { float4 ExposureVignetteChromatic; float4 HitFlash; float4 ScreenInfo; };
struct PSInput { float4 position : SV_POSITION; float4 color : COLOR0; float2 uv0 : TEXCOORD0; };
float4 main(PSInput input) : SV_TARGET
{
    float2 direction = input.uv0 - ScreenInfo.xy;
    float2 chroma = direction * ExposureVignetteChromatic.w * ScreenInfo.z;
    float3 color = float3(texture0.Sample(sampler0, input.uv0 + chroma).r, texture0.Sample(sampler0, input.uv0).g, texture0.Sample(sampler0, input.uv0 - chroma).b);
    color *= ExposureVignetteChromatic.x;
    color /= 1.0 + color;
    float softness = max(ExposureVignetteChromatic.z, 0.0001);
    float vignette = saturate((length(direction) * 1.4142136 - (1.0 - softness)) / softness);
    color *= 1.0 - vignette * ExposureVignetteChromatic.y;
    color = saturate(color + HitFlash.rgb * HitFlash.a);
    return float4(color, texture0.Sample(sampler0, input.uv0).a) * input.color;
}
