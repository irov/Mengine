float u_h;
float u_s;
float u_v;

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

float3 hsv2rgb( float3 HSV )
{
    float4 K = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    float3 p = abs( frac( HSV.xxx + K.xyz ) * 6.0 - K.www );

    return HSV.z * lerp( K.xxx, clamp( p - K.xxx, 0.0, 1.0 ), HSV.y );
}

void main( in v2p IN, out p2f OUT )
{
    float4 color = tex0.Sample( sampler0, IN.tex0 );

    float3 hsv = float3(u_h, u_s, u_v);
    float3 rgb = hsv2rgb( hsv );

    OUT.color = IN.color * color * float4(rgb, 1.0);
}
