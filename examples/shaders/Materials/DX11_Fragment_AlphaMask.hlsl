Texture2D tex0 : register(t0);
Texture2D tex1 : register(t1);
SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);

struct v2p
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
    float2 tex0 : TEXCOORD0;
    float2 tex1 : TEXCOORD1;
};

struct p2f
{
    float4 color : COLOR0;
};

void main( in v2p IN, out p2f OUT )
{
    float4 color0 = tex0.Sample( sampler0, IN.tex0 );
    float4 color1 = tex1.Sample( sampler1, IN.tex1 );

    OUT.color = IN.color * color0;
    OUT.color.a = IN.color.a * color1.a;
}
