#pragma once
#include "GameObject.h"

#include "Animation.h"
#include "Animations.h"

#include "Debug.h"

#define MARCO_WALKING_SPEED		0.1f

#define MARCO_JUMP_SPEED_Y		0.5f

#define MARCO_GRAVITY			0.002f

#define MARCO_JUMP_DEFLECT_SPEED  0.4f

#define MARCO_STATE_DIE				-10
#define MARCO_STATE_IDLE			0
#define MARCO_STATE_WALKING_RIGHT	100
#define MARCO_STATE_WALKING_LEFT	200

#define MARCO_STATE_JUMP			300
#define MARCO_STATE_RELEASE_JUMP    301

#define MARCO_STATE_SIT				600
#define MARCO_STATE_SIT_RELEASE		601


#pragma region ANIMATION_ID

#define ID_ANI_MARCO_IDLE_RIGHT 400
#define ID_ANI_MARCO_IDLE_LEFT 401

#define ID_ANI_MARCO_WALKING_RIGHT 500
#define ID_ANI_MARCO_WALKING_LEFT 501

#define ID_ANI_MARCO_JUMP_WALK_RIGHT 700
#define ID_ANI_MARCO_JUMP_WALK_LEFT 701

#define ID_ANI_MARCO_SIT_RIGHT 900
#define ID_ANI_MARCO_SIT_LEFT 901

#define ID_ANI_MARCO_DIE 999

#pragma endregion

#define GROUND_Y 160.0f

#define MARCO_BIG_BBOX_WIDTH  30
#define MARCO_BIG_BBOX_HEIGHT 38
#define MARCO_BIG_SITTING_BBOX_WIDTH  34
#define MARCO_BIG_SITTING_BBOX_HEIGHT 24

#define MARCO_SIT_HEIGHT_ADJUST ((MARCO_BIG_BBOX_HEIGHT-MARCO_BIG_SITTING_BBOX_HEIGHT)/2)

#define MARCO_SMALL_BBOX_WIDTH  13
#define MARCO_SMALL_BBOX_HEIGHT 12

#define MARCO_UNTOUCHABLE_TIME 2500

class CMARCO : public CGameObject
{
	BOOLEAN isSitting;
	float maxVx;
	float ax;				// acceleration on x 
	float ay;				// acceleration on y 

	int level;
	int untouchable;
	ULONGLONG untouchable_start;
	BOOLEAN isOnPlatform;
	int coin;/*

	void OnCollisionWithGoomba(LPCOLLISIONEVENT e);
	void OnCollisionWithCoin(LPCOLLISIONEVENT e);
	void OnCollisionWithPortal(LPCOLLISIONEVENT e);*/

public:
	CMARCO(float x, float y) : CGameObject(x, y)
	{
		isSitting = false;
		maxVx = 0.0f;
		ax = 0.0f;
		ay = MARCO_GRAVITY;

		untouchable = 0;
		untouchable_start = -1;
		isOnPlatform = false;
		coin = 0;
	}
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void Render();
	void SetState(int state);

	int IsCollidable()
	{
		return (state != MARCO_STATE_DIE);
	}

	int IsBlocking() { return (state != MARCO_STATE_DIE && untouchable == 0); }

	void OnNoCollision(DWORD dt);
	void OnCollisionWith(LPCOLLISIONEVENT e);

	int GetAniId();

	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount64(); }

	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
};