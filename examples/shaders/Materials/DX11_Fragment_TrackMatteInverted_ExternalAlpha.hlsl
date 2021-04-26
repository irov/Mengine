float4 uvbb;

Texture2D tex0 : register(t0);
Texture2D tex1 : register(t1);
Texture2D tex2 : register(t2);
SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);
SamplerState sampler2 : register(s2);

struct v2p
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

struct p2f
{
    float4 color : COLOR0;
};

void main( in v2p IN, out p2f OUT )
{
    float2 uv1_clamp;
    uv1_clamp.x = clamp( IN.uv1.x, uvbb.x, uvbb.z );
    uv1_clamp.y = clamp( IN.uv1.y, uvbb.y, uvbb.w );

    float4 color0 = tex0.Sample( sampler0, IN.uv0 );
    float4 alpha0 = tex1.Sample( sampler1, IN.uv0 );
    float4 color1 = tex2.Sample( sampler2, uv1_clamp );

    OUT.color = IN.color * color0;
    OUT.color.a *= alpha0.a;
    OUT.color.a *= 1.0 - color1.a;
}
