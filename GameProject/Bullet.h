#pragma once
#include "GameObject.h"

#define BULLET_SPEED 1.0f

#define BULLET_BBOX_WIDTH 3
#define BULLET_BBOX_HEIGHT 3

#define BULLET_DIE_TIMEOUT 100

#define ID_ANI_BULLET 8000

class Bullet : public CGameObject
{
protected:
	float ax;
	float ay;

	ULONGLONG die_start;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual void OnNoCollision(DWORD dt);

	virtual void OnCollisionWith(LPCOLLISIONEVENT e);

public:
	Bullet(float x, float y);
};

