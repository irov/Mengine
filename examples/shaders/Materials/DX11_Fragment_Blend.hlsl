// textures
Texture2D	tex0 : register(t0);
SamplerState sampler0 : register(s0);

struct v2p {
    float4 position  : SV_POSITION;
    float4 color     : COLOR0;
    float2 uv0       : TEXCOORD0;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	min16float4 pos     : SV_POSITION;
	min16float4 color   : COLOR0;
	min16float2 uv0     : TEXCOORD0;
};

struct PixelShaderOutput {
	min16float4 color : COLOR0;
};
  
void main(in PixelShaderInput IN, out PixelShaderOutput OUT)
{
	//Lookup texture color
	min16float4 TexColor = tex0.Sample(sampler0, IN.uv0);
    OUT.color = IN.color * TexColor;
}
