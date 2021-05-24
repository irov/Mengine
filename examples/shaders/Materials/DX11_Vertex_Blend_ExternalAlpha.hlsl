cbuffer ModelConstantBuffer : register(b0)
{
    float4x4 ModelViewMatrix;
};

struct a2v
{
    float4 pos : POSITION;
    float4 color : COLOR0;
    float2 tex0 : TEXCOORD0;
    float2 tex1 : TEXCOORD1;
};

struct v2p
{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
    float2 tex0 : TEXCOORD0;
    float2 tex1 : TEXCOORD1;
};

void main( in a2v IN, out v2p OUT )
{
    OUT.pos = mul( IN.pos, ModelViewMatrix );
    OUT.color = IN.color;
    OUT.tex0 = IN.tex0;
    OUT.tex1 = IN.tex1;
}

