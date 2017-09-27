#pragma once

#include <DirectXMath.h>

#define MAX_LIGHTS 8

enum LightType
{
	NO_LIGHT, DIRECTIONAL_LIGHT, POINT_LIGHT, SPOT_LIGHT
};

struct Light
{
	DirectX::XMFLOAT4 diffuseColor;
	DirectX::XMFLOAT3 direction;
	float brightness;
	DirectX::XMFLOAT3 position;
	float specularity;
	float radius;
	float spotAngle;
	float spotFalloff;
	LightType lightType;
};