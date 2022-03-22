/* =============================================================
	INTRODUCTION TO GAME PROGRAMMING SE102
	
	SAMPLE 04 - COLLISION

	This sample illustrates how to:

		1/ Implement SweptAABB algorithm between moving objects
		2/ Implement a simple (yet effective) collision frame work

	Key functions: 
		CGame::SweptAABB
		CGameObject::SweptAABBEx
		CGameObject::CalcPotentialCollisions
		CGameObject::FilterCollision

		CGameObject::GetBoundingBox
		
================================================================ */

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "debug.h"
#include "Game.h"
#include "GameObject.h"
#include "Textures.h"

#include "character.h"
#include "Brick.h"
#include "Solider.h"

#define WINDOW_CLASS_NAME L"METAL SLUG 3 PROJECT"
#define MAIN_WINDOW_TITLE L"METAL SLUG 3"

#define BACKGROUND_COLOR D3DCOLOR_XRGB(255, 255, 220)
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 260

#define MAX_FRAME_RATE 120

#define ID_TEX_character 0
#define ID_TEX_ENEMY_LEFT 10
#define ID_TEX_ENEMY_RIGHT 15
#define ID_TEX_MISC 20
#define ID_TEX_MAP 30
#define ID_TEX_CAVE 40
CGame *game;

Ccharacter *character;
CSolider *Solider;

vector<LPGAMEOBJECT> objects;

class CSampleKeyHander: public CKeyEventHandler
{
	virtual void KeyState(BYTE *states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
};

CSampleKeyHander * keyHandler; 

void CSampleKeyHander::OnKeyDown(int KeyCode)
{
	DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
	switch (KeyCode)
	{
	case DIK_UP:
		character->SetState(character_STATE_JUMP);
		break;
	case DIK_R: // reset
		character->SetState(character_STATE_IDLE);
		character->SetLevel(character_LEVEL_BIG);
		character->SetPosition(-150.0f,0.0f);
		character->SetSpeed(0, 0);
		break;
	}
}

void CSampleKeyHander::OnKeyUp(int KeyCode)
{
	DebugOut(L"[INFO] KeyUp: %d\n", KeyCode);
}

void CSampleKeyHander::KeyState(BYTE *states)
{
	// disable control key when character die 
	if (character->GetState() == character_STATE_DIE) return;
	if (game->IsKeyDown(DIK_RIGHT))
		character->SetState(character_STATE_WALKING_RIGHT);
	else if (game->IsKeyDown(DIK_LEFT))
		character->SetState(character_STATE_WALKING_LEFT);
	else if (game->IsKeyDown(DIK_UP))
		character->SetState(character_STATE_JUMP);
	else
		character->SetState(character_STATE_IDLE);
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

/*
	Load all game resources 
	In this example: load textures, sprites, animations and character object

	TO-DO: Improve this function by loading texture,sprite,animation,object from file
*/
void LoadResources()
{
	CTextures * textures = CTextures::GetInstance();

	textures->Add(ID_TEX_character, L"textures\\rambo.png",D3DCOLOR_XRGB(255, 255, 255));
	textures->Add(ID_TEX_MISC, L"textures\\mic.png", D3DCOLOR_XRGB(176, 224, 248));
	textures->Add(ID_TEX_MAP, L"textures\\Mission1.png", D3DCOLOR_XRGB(248, 0, 248));
	textures->Add(ID_TEX_ENEMY_LEFT, L"textures\\left.png", D3DCOLOR_XRGB(223, 26, 110));
	textures->Add(ID_TEX_ENEMY_RIGHT, L"textures\\right.png", D3DCOLOR_XRGB(223, 26, 110));
	textures->Add(ID_TEX_CAVE, L"textures\\idle.png", D3DCOLOR_XRGB(255, 255, 255));

	textures->Add(ID_TEX_BBOX, L"textures\\bbox.png", D3DCOLOR_XRGB(255, 255, 255));


	CSprites * sprites = CSprites::GetInstance();
	CAnimations * animations = CAnimations::GetInstance();
	
	LPDIRECT3DTEXTURE9 texcharacter = textures->Get(ID_TEX_character);

	// big
	sprites->Add(10001, 507, 1, 537, 41, texcharacter);		// idle right

	sprites->Add(10002, 539, 1,567 , 42, texcharacter);		// walk
	sprites->Add(10003, 568, 1, 599, 41, texcharacter);
	sprites->Add(10004, 600, 1, 632, 42, texcharacter);
	sprites->Add(10005, 634, 1, 661, 41, texcharacter);
	sprites->Add(10006, 665, 1, 694, 41, texcharacter);

	sprites->Add(10011, 263, 1, 293, 41, texcharacter);		// idle left
	sprites->Add(10012, 233, 1, 261, 42, texcharacter);		// walk
	sprites->Add(10013, 200, 1, 232, 41, texcharacter);
	sprites->Add(10014, 167, 1, 199, 42, texcharacter);
	sprites->Add(10015, 138, 1, 165, 41, texcharacter);

	sprites->Add(10041, 308, 191, 337, 235, texcharacter); //jump left 
	sprites->Add(10051, 463, 191, 492, 232, texcharacter); //jump right

	sprites->Add(10071, 308, 191, 337, 235, texcharacter);

	sprites->Add(10099, 215, 120, 231, 135, texcharacter);		// die 

	// small
	sprites->Add(10021, 247, 0, 259, 15, texcharacter);			// idle small right
	sprites->Add(10022, 275, 0, 291, 15, texcharacter);			// walk 
	sprites->Add(10023, 306, 0, 320, 15, texcharacter);			// 

	sprites->Add(10031, 187, 0, 198, 15, texcharacter);			// idle small left

	sprites->Add(10032, 155, 0, 170, 15, texcharacter);			// walk
	sprites->Add(10033, 125, 0, 139, 15, texcharacter);			// 

	LPDIRECT3DTEXTURE9 texMap = textures->Get(ID_TEX_MAP);
	sprites->Add(20002, 2, 2, 2375, 274, texMap);

	LPDIRECT3DTEXTURE9 texGround = textures->Get(ID_TEX_MAP);
	sprites->Add(20003, 1, 1193, 1610, 1422, texGround);

	LPDIRECT3DTEXTURE9 texCave = textures->Get(ID_TEX_MAP);
	sprites->Add(90001, 3596, 1, 3860, 211, texCave);

	LPDIRECT3DTEXTURE9 texMisc = textures->Get(ID_TEX_MISC);
	sprites->Add(20001, 408, 225, 424, 241, texMisc);

	LPDIRECT3DTEXTURE9 texEnemy1 = textures->Get(ID_TEX_ENEMY_RIGHT);
	sprites->Add(30001, 444, 55, 485, 94, texEnemy1);//walk_right
	sprites->Add(30002, 400, 54, 441, 94, texEnemy1);
	sprites->Add(30003, 357, 53, 397, 93, texEnemy1);
	sprites->Add(30004, 313, 54, 354, 93, texEnemy1);

	LPDIRECT3DTEXTURE9 texEnemy = textures->Get(ID_TEX_ENEMY_LEFT);
	sprites->Add(30011, 221, 55, 262, 94, texEnemy);//walk_left
	sprites->Add(30012, 178, 56, 218, 94, texEnemy);
	sprites->Add(30013, 134, 54, 175, 93, texEnemy);
	sprites->Add(30014, 91, 53, 131, 93, texEnemy);

	sprites->Add(30099, 236, 1266, 264, 1284, texEnemy); // die sprite

	LPANIMATION ani;

	ani = new CAnimation(100);	// idle big right
	ani->Add(10001);
	animations->Add(400, ani);

	ani = new CAnimation(100);	// idle big left
	ani->Add(10011);
	animations->Add(401, ani);

	ani = new CAnimation(100);	// idle small right
	ani->Add(10021);
	animations->Add(402, ani);

	ani = new CAnimation(100);	// idle small left
	ani->Add(10031);
	animations->Add(403, ani);

	ani = new CAnimation(100);	// walk right big
	ani->Add(10001);
	ani->Add(10002);
	ani->Add(10003);
	ani->Add(10004);
	ani->Add(10005);
	ani->Add(10006);
	animations->Add(500, ani);

	ani = new CAnimation(100);	// // walk left big
	ani->Add(10011);
	ani->Add(10012);
	ani->Add(10013);
	ani->Add(10014);
	ani->Add(10015);
	animations->Add(501, ani);

	ani = new CAnimation(100);	// walk right small
	ani->Add(10021);
	ani->Add(10022);
	ani->Add(10023);
	animations->Add(502, ani);

	ani = new CAnimation(100);	// walk left small
	ani->Add(10031);
	ani->Add(10032);
	ani->Add(10033);
	animations->Add(503, ani);

	ani = new CAnimation(100);	// jump left
	ani->Add(10041);
	animations->Add(514, ani);

	ani = new CAnimation(100);	// jump right 
	ani->Add(10051);
	animations->Add(515, ani);



	ani = new CAnimation(100);		// character die
	ani->Add(10099);
	animations->Add(599, ani);

	

	ani = new CAnimation(100);		// brick
	ani->Add(20001);
	animations->Add(601, ani);

	ani = new CAnimation(300);		// Solider walk right
	ani->Add(30001);
	ani->Add(30002);
	ani->Add(30003);
	ani->Add(30004);
	animations->Add(700, ani);

	ani = new CAnimation(300);		// Solider walk left
	ani->Add(30011);
	ani->Add(30012);
	ani->Add(30013);
	ani->Add(30014);
	animations->Add(701, ani);

	ani = new CAnimation(1000);		// Solider dead
	ani->Add(30099);
	animations->Add(799, ani);

	ani = new CAnimation(900);		
	ani->Add(20002);
	animations->Add(602, ani);

	ani = new CAnimation(900);
	ani->Add(20003);
	animations->Add(603, ani);

	ani = new CAnimation(900);
	ani->Add(90001);
	animations->Add(910, ani);

	CBrick* map = new CBrick();
	map->AddAnimation(602);
	map->SetPosition(-250, -65);
	objects.push_back(map);

	CBrick* ground = new CBrick();
	ground->AddAnimation(603);
	ground->SetPosition(-250, -2);
	objects.push_back(ground);

	CBrick* cave = new CBrick();
	cave->AddAnimation(910);
	cave->SetPosition(415, 13);
	objects.push_back(cave);

	character = new Ccharacter();
	character->AddAnimation(400);		// idle right big
	character->AddAnimation(401);		// idle left big
	character->AddAnimation(402);		// idle right small
	character->AddAnimation(403);		// idle left small

	character->AddAnimation(500);		// walk right big
	character->AddAnimation(501);		// walk left big
	character->AddAnimation(502);		// walk right small
	character->AddAnimation(503);		// walk left big
	character->AddAnimation(514);
	character->AddAnimation(515);

	character->AddAnimation(599);		// die

	character->SetPosition(-150.0f, 0);
	objects.push_back(character);

	//for (int i = 0; i < 3; i++)
	//{
	//	CBrick *brick = new CBrick();
	//	brick->AddAnimation(601);
	//	brick->SetPosition(100.0f + i*60.0f, 45.0f);
	//	objects.push_back(brick);

	//	brick = new CBrick();
	//	brick->AddAnimation(601);
	//	brick->SetPosition(100.0f + i*60.0f, 60.0f);
	//	objects.push_back(brick);

	//	brick = new CBrick();
	//	brick->AddAnimation(601);
	//	brick->SetPosition(84.0f + i*60.0f, 60.0f);
	//	objects.push_back(brick);
	//}
	for (int i = -15; i < 5; i++)
	{
		CBrick* brick = new CBrick();
		brick->AddAnimation(601);
		brick->SetPosition(i * 16.0f, 200);
		objects.push_back(brick);
	}
	for (int i = 5; i < 10; i++)
	{
		CBrick* brick = new CBrick();
		brick->AddAnimation(601);
		brick->SetPosition(i * 16.0f, 200 - i*2);
		objects.push_back(brick);
	}
	for (int i = 10; i < 20; i++)
	{
		CBrick* brick = new CBrick();
		brick->AddAnimation(601);
		brick->SetPosition(i * 16.0f, 180);
		objects.push_back(brick);
	}
	for (int i = 20; i < 30; i++)
	{
		CBrick* brick = new CBrick();
		brick->AddAnimation(601);
		brick->SetPosition(i * 16.0f, 180 - i%20*4);
		objects.push_back(brick);
	}
	for (int i = 30; i < 35; i++)
	{
		CBrick* brick = new CBrick();
		brick->AddAnimation(601);
		brick->SetPosition(i * 16.0f, 140);
		objects.push_back(brick);
	}
	for (int i = 35; i < 45; i++)
	{
		CBrick* brick = new CBrick();
		brick->AddAnimation(601);
		brick->SetPosition(i * 16.0f, 140 + i%35*7);
		objects.push_back(brick);
	}
	for (int i = 45; i < 100; i++)
	{
		CBrick* brick = new CBrick();
		brick->AddAnimation(601);
		brick->SetPosition(i * 16.0f, 210);
		objects.push_back(brick);
	}
	////////Brick on sky
	//for (int i = 5; i < 10; i++)
	//{
	//	CBrick* brick = new CBrick();
	//	brick->AddAnimation(601);
	//	brick->SetPosition(i * 16.0f, 65);
	//	objects.push_back(brick);
	//}
	// and Soliders 
	//for (int i = 0; i < 2; i++)
	//{
	//	Solider = new CSolider();
	//	Solider->AddAnimation(700);
	//	Solider->AddAnimation(701);
	//	Solider->AddAnimation(799);
	//	Solider->SetPosition(200 + i*70, 215);
	//	Solider->SetState(Solider_STATE_WALKING_LEFT);
	//	objects.push_back(Solider);
	//}

}

/*
	Update world status for this frame
	dt: time period between beginning of last frame and beginning of this frame
*/
void Update(DWORD dt)
{
	// We know that character is the first object in the list hence we won't add him into the colliable object list
	// TO-DO: This is a "dirty" way, need a more organized way 

	vector<LPGAMEOBJECT> coObjects;
	for (int i = 1; i < objects.size(); i++)
	{
		coObjects.push_back(objects[i]);
	}

	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->Update(dt, &coObjects);
	}


	// Update camera to follow character
	float cx, cy;
	character->GetPosition(cx, cy);
	if (cx > 69.9 * 30.0f)
	{
		cx = 69.8 * 30.0f- (SCREEN_WIDTH / 2);
		cy -= SCREEN_HEIGHT / 2;
	}
	else if (cx < -2.5 * 30.0f)
	{
		cx =-2.4 * 30.0f - (SCREEN_WIDTH / 2);
		cy -= SCREEN_HEIGHT / 2;
	}
	else
	{
		cx -= SCREEN_WIDTH / 2;
		cy -= SCREEN_HEIGHT / 2;
	}
	CGame::GetInstance()->SetCamPos(cx, 0.0f /*cy*/);
}

/*
	Render a frame 
*/
void Render()
{
	LPDIRECT3DDEVICE9 d3ddv = game->GetDirect3DDevice();
	LPDIRECT3DSURFACE9 bb = game->GetBackBuffer();
	LPD3DXSPRITE spriteHandler = game->GetSpriteHandler();

	if (d3ddv->BeginScene())
	{
		// Clear back buffer with a color
		d3ddv->ColorFill(bb, NULL, BACKGROUND_COLOR);

		spriteHandler->Begin(D3DXSPRITE_ALPHABLEND);

		for (int i = 0; i < objects.size(); i++)
			objects[i]->Render();

		spriteHandler->End();
		d3ddv->EndScene();
	}

	// Display back buffer content to the screen
	d3ddv->Present(NULL, NULL, NULL, NULL);
}

HWND CreateGameWindow(HINSTANCE hInstance, int nCmdShow, int ScreenWidth, int ScreenHeight)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = hInstance;

	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WINDOW_CLASS_NAME;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	HWND hWnd =
		CreateWindow(
			WINDOW_CLASS_NAME,
			MAIN_WINDOW_TITLE,
			WS_OVERLAPPEDWINDOW, // WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			ScreenWidth,
			ScreenHeight,
			NULL,
			NULL,
			hInstance,
			NULL);

	if (!hWnd) 
	{
		OutputDebugString(L"[ERROR] CreateWindow failed");
		DWORD ErrCode = GetLastError();
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}

int Run()
{
	MSG msg;
	int done = 0;
	DWORD frameStart = GetTickCount();
	DWORD tickPerFrame = 1000 / MAX_FRAME_RATE;

	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) done = 1;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		DWORD now = GetTickCount();

		// dt: the time between (beginning of last frame) and now
		// this frame: the frame we are about to render
		DWORD dt = now - frameStart;

		if (dt >= tickPerFrame)
		{
			frameStart = now;

			game->ProcessKeyboard();
			
			Update(dt);
			Render();
		}
		else
			Sleep(tickPerFrame - dt);	
	}

	return 1;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd = CreateGameWindow(hInstance, nCmdShow, SCREEN_WIDTH, SCREEN_HEIGHT);

	game = CGame::GetInstance();
	game->Init(hWnd);

	keyHandler = new CSampleKeyHander();
	game->InitKeyboard(keyHandler);


	LoadResources();

	SetWindowPos(hWnd, 0, 0, 0, SCREEN_WIDTH*2, SCREEN_HEIGHT*2, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

	Run();

	return 0;
}