float3 packFloat3(float3 input)
{
	return input * 0.5f + 0.5f;
}

float3 unpackFloat3(float3 input)
{
	return input * 2 - 1;
}

float2 packFloat2(float2 input)
{
	return input * 0.5f + 0.5f;
}

float2 unpackFloat2(float2 input)
{
	return input * 2 - 1;
}