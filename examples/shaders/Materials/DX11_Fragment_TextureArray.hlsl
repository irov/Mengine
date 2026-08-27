Texture2DArray tex0 : register(t0);
SamplerState sampler0 : register(s0);

struct v2p
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
    float2 tex0 : TEXCOORD0;
    nointerpolation float layer : TEXCOORD1;
};

struct p2f
{
    float4 color : COLOR0;
};

void main( in v2p IN, out p2f OUT )
{
    float4 sampled = tex0.Sample( sampler0, float3( IN.tex0, IN.layer ) );
    OUT.color = IN.color * sampled;
}
