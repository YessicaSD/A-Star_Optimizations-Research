#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1PathFinding.h"
#include "j1Scene.h"

#include <vector>

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	if(App->map->Load("test_map.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;
		if(App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	//debug_tex = App->tex->Load("maps/debugTex.png");

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	// Debug Pathfinding ------------------
	static iPoint origin;
	static bool origin_selected = false;
	static bool erasing = false; 

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint tileMouse = App->render->ScreenToWorld(x, y);
	tileMouse = App->map->WorldToMap(tileMouse.x, tileMouse.y);

	if(App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		if(origin_selected == true)
		{
			App->pathfinding->CreatePath(origin, tileMouse);
			origin_selected = false;
		}
		else
		{
			origin = tileMouse;
			origin_selected = true;
		}
	}

	// Changes Walkability Map - Needs Revision
	// ------------------------------------------
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (!App->pathfinding->IsWalkable(tileMouse))
		{
			erasing = true;
			//LOG("Erasing enabled");
		}
		else
		{
			erasing = false;
			//LOG("Erasing disabled");
		}
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		if (erasing)
			App->pathfinding->DeactivateTile(tileMouse); 
		else App->pathfinding->ActivateTile(tileMouse);
	}

	// ------------------------------------------

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		App->pathfinding->CycleJPS(); 
	}

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += 1;

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= 1;

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += 1;

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= 1;
	
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		App->win->SetScale(4);
			
	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
		App->win->SetScale(3);

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
		App->win->SetScale(2);

	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
		App->win->SetScale(1);


	App->map->Draw();

	int x, y;
	App->input->GetMousePosition(x, y);
	int x2 = x;
	int y2 = y; 
	iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);
	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Tile:%d,%d WorldPos: %i,%i",
					App->map->data.width, App->map->data.height,
					App->map->data.tile_width, App->map->data.tile_height,
					App->map->data.tilesets.count(),
					map_coordinates.x, map_coordinates.y,
					x2,y2);

	App->win->SetTitle(title.GetString());

	// Debug pathfinding ------------------------------
	//int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);
	p = App->map->MapToWorld(p.x, p.y);

	SDL_Rect rect; 
	rect.x = p.x;
	rect.y = p.y; 
	rect.w = App->map->data.tile_width;
	rect.h = App->map->data.tile_height;
	App->render->DrawQuad(rect,255,0,255,255);

	std::vector<iPoint> path = *App->pathfinding->GetLastPath();

	for(uint i = 0; i < path.size(); ++i)
	{
		iPoint pos = App->map->MapToWorld(path[i].x, path[i].y);		// X + 1, Same problem with map
		App->render->DrawQuad({pos.x, pos.y, App->map->data.tile_width, App->map->data.tile_height}, 255, 100,255);
		//App->render->DrawQuad({ pos.x, pos.y + (int)(App->map->data.tile_height * 0.5F), 5, 5 }, 255, 0, 255, 175, true);
		//App->render->Blit(debug_tex, pos.x, pos.y);
	}

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
