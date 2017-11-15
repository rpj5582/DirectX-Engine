cbuffer matrices : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
}

float4 main(float2 inputPosition : POSITION) : SV_POSITION
{
	matrix mvp = mul(mul(world, view), projection);
	float4 position = mul(float4(inputPosition, 0.0f, 1.0f), mvp);
	return position;
}