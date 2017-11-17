cbuffer matrices : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
}

struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float3 position		: POSITION;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float3 barycentric	: BARYCENTRIC;
};

float4 main(VertexShaderInput input) : SV_POSITION
{
	matrix mvp = mul(mul(world, view), projection);
	float4 position = mul(float4(input.position, 1.0f), mvp);
	return position;
}