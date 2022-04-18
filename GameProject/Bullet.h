#pragma once
#include "GameObject.h"

#define BULLET_SPEED 1.0f

#define BULLET_BBOX_WIDTH 16
#define BULLET_BBOX_HEIGHT 16

#define ID_ANI_BULLET 8000

class Bullet : public CGameObject
{
protected:
	float ax;
	float ay;

	int ammo;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual void OnNoCollision(DWORD dt);

	virtual void OnCollisionWith(LPCOLLISIONEVENT e); 
	void OnCollisionWithChowmeinConga(LPCOLLISIONEVENT e);

public:
	Bullet(float x, float y);
};

