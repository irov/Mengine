cbuffer CameraConstants : register(b0) { float4x4 ModelViewMatrix; };
struct VSInput { float4 position : POSITION; float4 color : COLOR0; float2 uv0 : TEXCOORD0; };
struct VSOutput { float4 position : SV_POSITION; float4 color : COLOR0; float2 uv0 : TEXCOORD0; };
void main(in VSInput input, out VSOutput output) { output.position = mul(input.position, ModelViewMatrix); output.color = input.color; output.uv0 = input.uv0; }
