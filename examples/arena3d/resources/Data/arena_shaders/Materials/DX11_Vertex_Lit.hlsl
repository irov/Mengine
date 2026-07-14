cbuffer CameraConstants : register(b0) { float4x4 ModelViewMatrix; };
cbuffer ArenaMeshConstants : register(b4) { float4x4 WorldMatrix; float4 Ambient; float4 Directional; float4 DirectionalColor; float4 PointPosition[4]; float4 PointColor[4]; };
struct VSInput { float4 position : POSITION; float3 normal : NORMAL; float4 tangent : TANGENT; float4 color : COLOR0; float2 uv0 : TEXCOORD0; float2 uv1 : TEXCOORD1; };
struct VSOutput { float4 position : SV_POSITION; float4 color : COLOR0; float2 uv0 : TEXCOORD0; };
void main(in VSInput input, out VSOutput output)
{
    output.position = mul(input.position, ModelViewMatrix);
    float3 worldPosition = mul(input.position, WorldMatrix).xyz;
    float3 worldNormal = normalize(mul(float4(input.normal, 0.0), WorldMatrix).xyz);
    float3 lighting = Ambient.xyz;
    if (Directional.w > 0.5) lighting += DirectionalColor.xyz * (DirectionalColor.w * max(0.0, dot(worldNormal, -Directional.xyz)));
    [unroll] for (int index = 0; index != 4; ++index) {
        float invRadius = PointPosition[index].w;
        float3 toLight = PointPosition[index].xyz - worldPosition;
        float distanceToLight = length(toLight);
        float attenuation = saturate(1.0 - distanceToLight * invRadius);
        attenuation *= attenuation;
        lighting += PointColor[index].xyz * (PointColor[index].w * max(0.0, dot(worldNormal, toLight / max(distanceToLight, 0.0001))) * attenuation * step(0.00001, invRadius));
    }
    output.color = float4(input.color.rgb * lighting, input.color.a);
    output.uv0 = input.uv0;
}
