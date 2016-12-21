Texture2D gTexture;
SamplerState samLinear : register(s0);

cbuffer WVPBuffer : register(b1)
{
	matrix WVP;
}

struct VertexIn
{
	float3 PosL    : POSITION;	
	float2 Tex     : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 Tex  : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosH = mul(float4(vin.PosL, 1.0f), WVP);
	vout.Tex = vin.Tex;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 c = gTexture.Sample(samLinear, pin.Tex).r;

	// draw as grayscale
	return float4(c.rrr, 1);
}