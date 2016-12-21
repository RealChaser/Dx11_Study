Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
	float3 Normal : NORMAL;
    float2 Tex : TEXCOORD0;
};

cbuffer WVPBuffer : register(b1)
{
	matrix WVP;
}

PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(input.Pos, WVP);
    output.Tex = input.Tex;
    
    return output;
}

float4 PS( PS_INPUT input) : SV_Target
{
    return txDiffuse.Sample( samLinear, input.Tex );
}
