float u_factor;
float4 u_color;

Texture2D tex0 : register(t0);
SamplerState sampler0 : register(s0);

struct v2p
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
    float2 tex0 : TEXCOORD0;
};

struct p2f
{
    float4 color : COLOR0;
};

void main( in v2p IN, out p2f OUT )
{
    float4 color = tex0.Sample( sampler0, IN.tex0 ) * float4(u_color.xyz, 1.0);

    float l = color.x * 0.3 + color.y * 0.59 + color.z * 0.11;
    float4 d = float4(l, l, l, color.w);

    OUT.color = IN.color * lerp( color, d, u_factor );
}
