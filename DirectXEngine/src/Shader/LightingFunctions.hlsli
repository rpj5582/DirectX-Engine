// A LOT of info about different rendering methods and shader structure here: https://www.3dgep.com/forward-plus/

#define POINT_LIGHT 0
#define DIRECTIONAL_LIGHT 1
#define SPOT_LIGHT 2

#define MAX_LIGHTS 64

// Struct representing a generic light
struct Light
{
	float4 color;
	float3 direction;
	float brightness;
	float3 position;
	float specularity;
	float radius;
	float spotAngle;
	bool enabled;
	int type;
	bool shadowMapEnabled;
	int shadowType;
	float2 padding;
};

float4 calculateDiffuse(Light light, float3 normal, float3 directionToLight)
{
	return saturate(dot(directionToLight, normal)) * light.color * light.brightness;
}

float4 calculateSpecular(Light light, float3 pixelPosition, float3 directionToCamera, float3 normal, float3 directionToLight)
{
	float3 halfway = normalize(directionToLight + directionToCamera);
	return pow(saturate(dot(halfway, normal)), light.specularity) * light.color * light.brightness;
}

float calculateAttenuation(Light light, float distanceToLight, float smoothness)
{
	return 1.0f - smoothstep(light.radius * smoothness, light.radius, distanceToLight);
}

float calculateSpotlightAttenuation(Light light, float3 directionToLight)
{
	float minCos = cos(radians(light.spotAngle));
	float maxCos = lerp(minCos, 1.0f, 0.5f);
	float cosAngle = dot(light.direction, -directionToLight);
	return smoothstep(minCos, maxCos, cosAngle);
}