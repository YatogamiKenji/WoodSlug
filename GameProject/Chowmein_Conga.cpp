#include "Chowmein_Conga.h"

Chowmein_Conga::Chowmein_Conga(float x, float y) :CGameObject(x, y)
{
	this->ax = 0;
	this->ay = CHOWMEIN_CONGA_GRAVITY;
	die_start = -1;
	SetState(CHOWMEIN_CONGA_STATE_WALKING);
}

void Chowmein_Conga::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - CHOWMEIN_CONGA_BBOX_WIDTH / 2;
	top = y - CHOWMEIN_CONGA_BBOX_HEIGHT / 2;
	right = left + CHOWMEIN_CONGA_BBOX_WIDTH;
	bottom = top + CHOWMEIN_CONGA_BBOX_HEIGHT;
}

void Chowmein_Conga::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
};

void Chowmein_Conga::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (!e->obj->IsBlocking()) return;
	if (dynamic_cast<Chowmein_Conga*>(e->obj)) return;

	if (e->ny != 0)
	{
		vy = 0;
	}
	else if (e->nx != 0)
	{
		vx = -vx;
	}
}

void Chowmein_Conga::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	vx += ax * dt;

	if ((state == CHOWMEIN_CONGA_STATE_DIE) && (GetTickCount64() - die_start > CHOWMEIN_CONGA_DIE_TIMEOUT))
	{
		isDeleted = true;
		return;
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}


void Chowmein_Conga::Render()
{
	int aniId = ID_ANI_CHOWMEIN_CONGA_WALKING;
	if (state == CHOWMEIN_CONGA_STATE_DIE)
	{
		aniId = ID_ANI_CHOWMEIN_CONGA_DIE;
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	RenderBoundingBox();
}

void Chowmein_Conga::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case CHOWMEIN_CONGA_STATE_DIE:
		die_start = GetTickCount64();
		vx = 0;
		vy = 0;
		ay = 0;
		break;
	case CHOWMEIN_CONGA_STATE_WALKING:
		vx = -CHOWMEIN_CONGA_WALKING_SPEED;
		break;
	}
}
