#include "../InspirationEngine/InspirationEngine.h"
#include "nlohmann/json.hpp"
#include "GameScene.h"
#include <fstream>

void GameScene::OnEnter()
{
	SDL_Renderer* sdl =
		IECore::GetMainWindow()->GetRenderer(1)->GetSDLRenderer();

	IECore::GetAtlas().Load("floors", "Data/floors.json", sdl);

	std::ifstream f("Data/world_test.json");
	if (!f.is_open())
		return;

	nlohmann::json j = nlohmann::json::parse(f, nullptr, false);
	if (j.is_discarded())
		return;

	auto& camJson = j["camera"];
	float zoom = camJson.value("zoom", 2.0f);
	float cx   = camJson.value("x", 0.0f);
	float cy   = camJson.value("y", 0.0f);
	float cz   = camJson.value("z", 0.0f);

	auto [stepX, stepY] = IECore::GetAtlas().GetTileStep("floors");
	int32_t tileW = camJson.value("tileWidth",  stepX > 0 ? stepX : 32);
	int32_t tileH = camJson.value("tileHeight", stepY > 0 ? stepY : 16);

	auto* camera = new IECameraIsometric(tileW, tileH);
	camera->SetZoom(zoom);
	camera->SetPosition(cx, cy, cz);
	camera->SetViewport(512, 512);
	SetCamera(camera);

	for (auto& obj : j["objects"])
	{
		auto* go = AddObject<IEStaticObject>();
		auto* t  = go->GetComponent<IETransformComponent>();
		t->SetX(obj.value("x", 0.0f));
		t->SetY(obj.value("y", 0.0f));
		t->SetZ(obj.value("z", 0.0f));

		if (obj.contains("tile"))
		{
			auto* tc = go->AddComponent<IETileComponent>();
			tc->SetAtlas(obj["tile"].value("atlas", ""));
			tc->SetTile(obj["tile"].value("tile",  ""));
		}
	}
}

void GameScene::OnExit()
{
	IECore::GetAtlas().Unload("floors");
}
