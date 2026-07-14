Texture2D texture0 : register(t0); SamplerState sampler0 : register(s0);
struct PSInput { float4 position : SV_POSITION; float4 color : COLOR0; float2 uv0 : TEXCOORD0; };
float4 main(PSInput input) : SV_TARGET { return input.color * texture0.Sample(sampler0, input.uv0); }
