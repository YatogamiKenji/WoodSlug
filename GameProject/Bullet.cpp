#include "Bullet.h"

Bullet::Bullet(float x, float y) : CGameObject(x, y)
{
	this->ax = BULLET_SPEED;
	this->ay = 0;
	die_start = -1;
}

void Bullet::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - BULLET_BBOX_WIDTH / 2;
	top = y - BULLET_BBOX_HEIGHT / 2;
	right = left + BULLET_BBOX_WIDTH;
	bottom = top + BULLET_BBOX_HEIGHT;
}

void Bullet::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
};

void Bullet::OnCollisionWith(LPCOLLISIONEVENT e)
{
	isDeleted = true;
}

void Bullet::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	vx += ax * dt;

	if ((GetTickCount64() - die_start > BULLET_DIE_TIMEOUT))
	{
		isDeleted = true;
		return;
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}


void Bullet::Render()
{
	int aniId = ID_ANI_BULLET;

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	RenderBoundingBox();
}
