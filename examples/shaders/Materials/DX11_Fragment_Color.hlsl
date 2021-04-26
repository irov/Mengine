struct v2p
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};

struct p2f
{
    float4 color : COLOR0;
};

void main( in v2p IN, out p2f OUT )
{
    OUT.color = IN.color;
}
