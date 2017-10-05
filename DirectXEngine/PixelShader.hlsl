#include "UtilityFunctions.hlsli"
#include "LightingFunctions.hlsli"

cbuffer lighting : register(b0)
{
	Light lights[MAX_LIGHTS];
	unsigned int lightCount;
}

cbuffer camera : register(b1)
{
	float3 cameraWorldPosition : CAMERA_POSITION;
}

Texture2D diffuseTexture : register(t0);
Texture2D specularTexture : register(t1);
Texture2D normalTexture : register(t2);
SamplerState materialSampler : register(s0);

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
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
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

	// Calculates a value - either 1 or 0 - to disable lighting or not.
	// If there are no lights in the scene, render the scene without shading.
	float disableShading = 1.0f - min(lightCount, 1.0f);

	// Calculates the lighting for each valid light
	float4 globalAmbient = float4(0.1f, 0.1f, 0.1f, 1.0f);
	float4 finalLightColor = globalAmbient  * diffuseColor + disableShading * float4(1.0f, 1.0f, 1.0f, 1.0f);
	for (unsigned int i = 0; i < lightCount; i++)
	{
		float4 lightColor = calculateLight(lights[i], finalNormal, input.worldPosition, diffuseColor, specularColor);
		finalLightColor += lightColor;
	}
	
	return finalLightColor;
}