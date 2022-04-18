#pragma once
#include "GameObject.h"

#define CHOWMEIN_CONGA_GRAVITY 0.002f
#define CHOWMEIN_CONGA_WALKING_SPEED 0.05f


#define CHOWMEIN_CONGA_BBOX_WIDTH 51
#define CHOWMEIN_CONGA_BBOX_HEIGHT 45

#define CHOWMEIN_CONGA_DIE_TIMEOUT 2000

#define CHOWMEIN_CONGA_STATE_WALKING 100
#define CHOWMEIN_CONGA_STATE_DIE 200

#define ID_ANI_CHOWMEIN_CONGA_IDLE 1000101
#define ID_ANI_CHOWMEIN_CONGA_WALKING 1000102
#define ID_ANI_CHOWMEIN_CONGA_DIE 1000108

class Chowmein_Conga : public CGameObject
{
protected:
	float ax;
	float ay;

	ULONGLONG die_start;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable() { return 1; };
	virtual int IsBlocking() { return 0; }
	virtual void OnNoCollision(DWORD dt);

	virtual void OnCollisionWith(LPCOLLISIONEVENT e);

public:
	Chowmein_Conga(float x, float y);
	virtual void SetState(int state);
};