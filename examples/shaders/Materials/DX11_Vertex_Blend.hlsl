cbuffer ModelConstantBuffer : register(b0)
{
	float4x4 ModelViewMatrix;
};
  
// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	min16float4 pos			: POSITION;
	min16float4 color		: COLOR0;
	min16float2 texcoords   : TEXCOORD0;
};

struct VertexShaderOutput
{
	min16float4 pos     : SV_POSITION;
	min16float4 color   : COLOR0;
	min16float2 uv0     : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput IN)
{
	VertexShaderOutput OUT;

    OUT.pos = mul(IN.pos, ModelViewMatrix);
    OUT.color = IN.color;
    OUT.tex0 = IN.tex0;
}

