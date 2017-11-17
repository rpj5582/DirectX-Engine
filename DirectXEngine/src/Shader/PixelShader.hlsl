#include "UtilityFunctions.hlsli"
#include "LightingFunctions.hlsli"

#define SOLID 0
#define WIREFRAME 1
#define SOLID_WIREFRAME 2

#define SHADOWMAP_SIZE 1024.0f

cbuffer lighting : register(b0)
{
	Light lights[MAX_LIGHTS];
}

cbuffer camera : register(b1)
{
	float3 cameraWorldPosition : CAMERA_POSITION;
}

cbuffer renderStyle : register(b2)
{
	int renderStyle;
	float4 wireColor;
}

Texture2D diffuseTexture : register(t0);
Texture2D specularTexture : register(t1);
Texture2D normalTexture : register(t2);
Texture2D shadowMap : register(t3);
SamplerState materialSampler : register(s0);
SamplerComparisonState shadowMapSampler : register(s1);

// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;
	float3 worldPosition : WORLD_POSITION;
	float4 shadowPosition : SHADOW_POSITION;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float3 barycentric	: BARYCENTRIC;
};

float4 calculateLight(Light light, float3 normal, float3 worldPosition, float4 surfaceColor, float4 specColor)
{
	if (!light.enabled)
	{
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	else
	{
		float3 directionToLight = float3(0.0f, 0.0f, 0.0f);
		float distanceToLight = 0.0f;
		float attenuation = 1.0f;
		float spotlightAttenuation = 1.0f;

		// Defines the direction and distance to the light from this pixel
		// as well as spot light attenuation based on the type of light
		switch (light.type)
		{
		case POINT_LIGHT:
			directionToLight = light.position - worldPosition;
			distanceToLight = length(directionToLight);
			directionToLight /= distanceToLight;

			attenuation = calculateAttenuation(light, distanceToLight, 0.75f);
			break;

		case DIRECTIONAL_LIGHT:
			directionToLight = normalize(-light.direction);
			break;

		case SPOT_LIGHT:
			directionToLight = light.position - worldPosition;
			distanceToLight = length(directionToLight);
			directionToLight /= distanceToLight;

			attenuation = calculateAttenuation(light, distanceToLight, 0.75f);
			spotlightAttenuation = calculateSpotlightAttenuation(light, directionToLight);
			break;

		default:
			break;
		}

		float3 directionToCamera = normalize(cameraWorldPosition - worldPosition);

		// Calculates all different types of lighting
		float4 diffuseColor = calculateDiffuse(light, normal, directionToLight) * surfaceColor;
		float4 specularColor = calculateSpecular(light, worldPosition, directionToCamera, normal, directionToLight) * specColor;

		return (diffuseColor + specularColor) * attenuation * spotlightAttenuation;
	}
}

// Calculates the lerp factor when shading wireframes.
// Based on the following blog post: http://codeflow.org/entries/2012/aug/02/easy-wireframe-display-with-barycentric-coordinates/
float edgeFactor(float3 barycentric, float width)
{
	float3 d = fwidth(barycentric);
	float3 a3 = smoothstep(float3(0.0f, 0.0f, 0.0f), d * width, barycentric);
	return min(min(a3.x, a3.y), a3.z);

}

// Calculates the amount a pixel is in shadow using PCF. Sample width must be an odd positive integer.
float calculateShadowAmount(float4 shadowPosition, const unsigned int sampleWidth)
{
	const float pixel = 1.0f / SHADOWMAP_SIZE;

	float distanceToLight = shadowPosition.z / shadowPosition.w;

	float2 shadowMapUV = packFloat2(shadowPosition.xy / shadowPosition.w);
	shadowMapUV.y = 1.0f - shadowMapUV.y;

	float totalShadowAmount = 0;
	float sampleRadius = sampleWidth / 2.0f;
	int lowerBound = ceil(-sampleRadius);
	int upperBound = floor(sampleRadius);

	[unroll]
	for (int i = lowerBound; i <= upperBound; i++)
	{
		[unroll]
		for (int j = lowerBound; j <= upperBound; j++)
		{
			totalShadowAmount += shadowMap.SampleCmpLevelZero(shadowMapSampler, shadowMapUV + float2(i, j) * pixel, distanceToLight);
		}
	}
	
	return totalShadowAmount / (sampleWidth * sampleWidth);
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	float3 normal = normalize(input.normal);
	float3 tangent = normalize(input.tangent);

	float4 diffuseColor = diffuseTexture.Sample(materialSampler, input.uv);
	float4 specularColor = specularTexture.Sample(materialSampler, input.uv);
	float4 normalColor = normalTexture.Sample(materialSampler, input.uv);
	float3 unpackedNormal = unpackFloat3(normalColor.xyz);

	// Construct the TBN matrix for conversion from tangent space to world space
	float3 N = normal;
	float3 T = normalize(tangent - normal * dot(tangent, normal));
	float3 B = cross(T, N);
	float3x3 TBN = float3x3(T, B, N);

	// Convert the normal from the normal map to world space.
	float3 finalNormal = normalize(mul(unpackedNormal, TBN));

	// Must be an odd positive integer
	const unsigned int shadowSampleWidth = 1;
	float shadowAmount = calculateShadowAmount(input.shadowPosition, shadowSampleWidth);

	// Calculates the lighting for each valid light
	float4 globalAmbient = float4(0.1f, 0.1f, 0.1f, 1.0f);
	float4 finalLightColor = globalAmbient  * diffuseColor;

	[unroll]
	for (unsigned int i = 0; i < MAX_LIGHTS; i++)
	{
		float4 lightColor = calculateLight(lights[i], finalNormal, input.worldPosition, diffuseColor, specularColor);
		finalLightColor += lightColor * shadowAmount;
	}

	switch (renderStyle)
	{
	case SOLID:
		return finalLightColor;

	case WIREFRAME:
		return float4(wireColor.r, wireColor.g, wireColor.b, wireColor.a - edgeFactor(input.barycentric, 2.0f));

	case SOLID_WIREFRAME:
		return lerp(wireColor, finalLightColor, edgeFactor(input.barycentric, 2.0f));

	default: // Invalid render style
		return float4(1.0f, 0.0f, 1.0f, 1.0f);
	}
}