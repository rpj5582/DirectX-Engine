#pragma once
#include "Scene.h"

class Scene2 : public Scene
{
public:
	Scene2(std::string name, std::string filepath);
	~Scene2();

	virtual void onLoad() override;
};

