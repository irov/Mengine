float u_h;
float u_s;
float u_l;

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

float3 hue2rgb( float H )
{
    float R = clamp( abs( H * 6.0 - 3.0 ) - 1.0, 0.0, 1.0 );
    float G = clamp( 2.0 - abs( H * 6.0 - 2.0 ), 0.0, 1.0 );
    float B = clamp( 2.0 - abs( H * 6.0 - 4.0 ), 0.0, 1.0 );

    return float3(R, G, B);
}

float3 hsl2rgb( float H, float S, float L )
{
    float3 RGB = hue2rgb( H );
    float C = (1.0 - abs( 2.0 * L - 1.0 )) * S;

    return (RGB - 0.5) * C + L;
}

void main( in v2p IN, out p2f OUT )
{
    float4 color = tex0.Sample( sampler0, IN.tex0 );

    float3 rgb = hsl2rgb( u_h, u_s, u_l );

    OUT.color = IN.color * color * float4(rgb, 1.0);
}
