#define MAX_LIGHTS 8

// Struct representing a generic light
struct Light
{
	float4 diffuseColor;
	float3 direction;
	float brightness;
	float3 position;
	float specularity;
	float radius;
	float spotAngle;
	float spotFalloff;
	unsigned int type;
};

cbuffer lighting : register(b0)
{
	Light lights[MAX_LIGHTS];
	unsigned int lightCount;
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
	float2 uv			: TEXCOORDS;
	float3 normal		: NORMAL;
	float3 cameraWorldPosition : CAMERA_POSITION;
};

// Calculates the diffuse lighting for all light types
float4 calculateDiffuse(Light light, VertexToPixel input, float3 directionToLight)
{
	return saturate(dot(directionToLight, input.normal)) * light.diffuseColor * light.brightness;
}

// Calculates the specular lighting for all light types
float4 calculateSpecular(Light light, VertexToPixel input, float3 directionToLight)
{
	float3 directionToCamera = normalize(input.cameraWorldPosition - input.worldPosition);
	float3 halfway = normalize(directionToLight + directionToCamera);
	return pow(saturate(dot(halfway, input.normal)), light.specularity);
}

float4 calculateLight(Light light, VertexToPixel input, float4 surfaceColor, float4 specColor)
{
	float3 directionToLight = float3(0.0f, 0.0f, 0.0f);
	float distanceToLight = 0.0f;
	float spotlightAttenuation = 1.0f;

	// Defines the direction and distance to the light from this pixel
	// as well as spot light attenuation based on the type of light
	switch (light.type)
	{
	case 0: // Point light
		distanceToLight = length((float3)light.position - input.worldPosition);
		directionToLight = ((float3)light.position - input.worldPosition) / distanceToLight;
		break;

	case 1: // Directional light
		directionToLight = normalize((float3) - light.direction);
		break;

	case 2: // Spot light
		distanceToLight = length((float3)light.position - input.worldPosition);
		directionToLight = ((float3)light.position - input.worldPosition) / distanceToLight;
		spotlightAttenuation = pow(max(0.0f, dot(-directionToLight, light.direction) / cos(light.spotAngle)), light.spotFalloff);
		break;

	default:
		break;
	}

	// Calculates all different types of lighting
	float4 diffuseColor = calculateDiffuse(light, input, directionToLight) * surfaceColor;
	float4 specularColor = calculateSpecular(light, input, directionToLight) * specColor;

	// Light falloff based on radius
	float attenuationFactor = 1.0f;
	float attenuation = light.radius / (1.0f + attenuationFactor * distanceToLight * distanceToLight);

	return (diffuseColor + specularColor) * attenuation * spotlightAttenuation;
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

	float4 surfaceColor = diffuseTexture.Sample(materialSampler, input.uv);
	float4 specularColor = specularTexture.Sample(materialSampler, input.uv);

	// Calculates a value - either 1 or 0 - to disable lighting or not.
	// If there are no lights in the scene, render the scene without shading.
	float disableShading = 1.0f - min(lightCount, 1.0f);

	// Calculates the lighting for each valid light
	float4 globalAmbient = float4(0.1f, 0.1f, 0.1f, 1.0f);
	float4 finalLightColor = globalAmbient * surfaceColor + disableShading * float4(1.0f, 1.0f, 1.0f, 1.0f);
	for (unsigned int i = 0; i < lightCount; i++)
	{
		finalLightColor += calculateLight(lights[i], input, surfaceColor, specularColor);
	}
	
	return finalLightColor;
}