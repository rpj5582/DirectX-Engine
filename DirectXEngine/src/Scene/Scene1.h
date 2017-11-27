#pragma once
#include "Scene.h"

class Scene1 : public Scene
{
public:
	Scene1(std::string name, std::string filepath);
	~Scene1();

protected:
	void onLoad() override;

private:

};