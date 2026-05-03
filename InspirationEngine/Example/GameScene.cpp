#include "../InspirationEngine/InspirationEngine.h"
#include "nlohmann/json.hpp"
#include "GameScene.h"
#include <fstream>

void GameScene::onEnter()
{
	SDL_Renderer* sdl =
		IECore::getMainWindow()->getRenderer(1)->getSDLRenderer();

	IECore::m_Atlas.load("floors", "../Data/floors.json", sdl);

	std::ifstream f("../Data/world_test.json");
	if (!f.is_open()) return;
	nlohmann::json j;
	try { f >> j; } catch (...) { return; }

	auto& camJson = j["camera"];
	float zoom = camJson.value("zoom", 2.0f);
	float cx   = camJson.value("x", 0.0f);
	float cy   = camJson.value("y", 0.0f);

	// atlas에서 tileStep 읽기 — world 좌표당 픽셀 stride 정의
	auto [stepX, stepY] = IECore::m_Atlas.getTileStep("floors");
	int32_t tileW = camJson.value("tileWidth",  stepX > 0 ? stepX : 32);
	int32_t tileH = camJson.value("tileHeight", stepY > 0 ? stepY : 19);

	auto* camera = new IECameraIsometric(tileW, tileH);
	camera->setZoom(zoom);
	camera->setPosition(cx, cy, 0.0f);
	camera->setViewport(512, 512);
	setCamera(camera);

	for (auto& obj : j["objects"])
	{
		auto* go = addObject<IEStaticObject>();
		auto* t  = go->getComponent<IETransformComponent>();
		t->x = obj.value("x", 0.0f);
		t->y = obj.value("y", 0.0f);
		t->z = obj.value("z", 0.0f);

		if (obj.contains("tile"))
		{
			auto* tc  = go->addComponent<IETileComponent>();
			tc->atlas = obj["tile"].value("atlas", "");
			tc->tile  = obj["tile"].value("tile",  "");
		}
	}
}

void GameScene::onExit()
{
	IECore::m_Atlas.unload("floors");
}
