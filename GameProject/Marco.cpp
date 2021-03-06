#include <algorithm>
#include "debug.h"

#include "Marco.h"
#include "Chowmein_Conga.h"
#include "Game.h"
#include "Portal.h"
#include "Bullet.h"
#include "PlayScene.h"

#include "Collision.h"

void CMARCO::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	vx += ax * dt;
	
	// reset untouchable timer if untouchable time has passed
	if (GetTickCount64() - untouchable_start > MARCO_UNTOUCHABLE_TIME)
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	isOnPlatform = false;

	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CMARCO::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
	DebugOut(L"x = %d\n", x);
	DebugOut(L"y = %d\n", y);
}

void CMARCO::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (e->ny != 0 && e->obj->IsBlocking())
	{
		vy = 0;
		if (e->ny < 0) isOnPlatform = true;
	}
	else
		if (e->nx != 0 && e->obj->IsBlocking())
		{
			vx = 0;
		}

	if (dynamic_cast<Chowmein_Conga*>(e->obj))
		OnCollisionWithChowmeinConga(e);
	//else if (dynamic_cast<CCoin*>(e->obj))
	//	OnCollisionWithCoin(e);
	else if (dynamic_cast<Portal*>(e->obj))
		OnCollisionWithPortal(e);
	DebugOut(L"x = %f\n", x);
	DebugOut(L"y = %f\n", y);
}

void CMARCO::OnCollisionWithChowmeinConga(LPCOLLISIONEVENT e)
{
	Chowmein_Conga* chowmein = dynamic_cast<Chowmein_Conga*>(e->obj);

	// jump on top >> kill Goomba and deflect a bit 
	if (e->ny < 0)
	{
		if (chowmein->GetState() != CHOWMEIN_CONGA_STATE_DIE)
		{
			chowmein->SetState(CHOWMEIN_CONGA_STATE_DIE);
			vy = -MARCO_JUMP_DEFLECT_SPEED;
		}
	}
	else // hit by Goomba
	{
		/*if (killing)
		{
				chowmein->SetState(CHOWMEIN_CONGA_STATE_DIE);
		}*/
	}
}
//
//void CMARCO::OnCollisionWithCoin(LPCOLLISIONEVENT e)
//{
//	e->obj->Delete();
//	coin++;
//}
//
void CMARCO::OnCollisionWithPortal(LPCOLLISIONEVENT e)
{
	Portal* p = (Portal*)e->obj;
	CGame::GetInstance()->InitiateSwitchScene(p->GetSceneId());
}


//
// Get animdation ID for big MARCO
//
int CMARCO::GetAniId()
{
	int aniId = -1;
	if (!isOnPlatform)
	{
		if (nx >= 0)
			aniId = ID_ANI_MARCO_JUMP_WALK_RIGHT;
		else
			aniId = ID_ANI_MARCO_JUMP_WALK_LEFT;
	}
	else
		if (isSitting)
		{
			if (nx > 0)
				aniId = ID_ANI_MARCO_SIT_RIGHT;
			else
				aniId = ID_ANI_MARCO_SIT_LEFT;
		}
		else
			if (vx == 0)
			{
				if (nx > 0) aniId = ID_ANI_MARCO_IDLE_RIGHT;
				else aniId = ID_ANI_MARCO_IDLE_LEFT;
			}
			else if (vx > 0)
			{
				aniId = ID_ANI_MARCO_WALKING_RIGHT;
			}
			else // vx < 0
			{
				aniId = ID_ANI_MARCO_WALKING_LEFT;
			}

	if (aniId == -1) aniId = ID_ANI_MARCO_IDLE_RIGHT;

	return aniId;
}

void CMARCO::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	int aniId = -1;

	if (state == MARCO_STATE_DIE)
		aniId = ID_ANI_MARCO_DIE;
	else if (state == MARCO_STATE_SHOOTING)
	{
		aniId = ID_ANI_MARCO_SHOOTING_RIGHT;
		animations->Get(aniId)->Render(x, y);
		aniId = ID_ANI_LEG;
		animations->Get(aniId)->Render(x - 5, y + 14);
		return;
	}
	else aniId = GetAniId();

	animations->Get(aniId)->Render(x, y);
	//RenderBoundingBox();

	//DebugOutTitle(L"Coins: %d", coin);
}

void CMARCO::SetState(int state)
{
	// DIE is the end state, cannot be changed! 
	if (this->state == MARCO_STATE_DIE) return;

	switch (state)
	{
	case MARCO_STATE_WALKING_RIGHT:
		if (isSitting) break;
		vx = MARCO_WALKING_SPEED;
		nx = 1;
		break;
	case MARCO_STATE_WALKING_LEFT:
		if (isSitting) break;
		vx = -MARCO_WALKING_SPEED;
		nx = -1;
		break;
	case MARCO_STATE_JUMP:
		if (isSitting) break;
		if (isOnPlatform)
		{
			vy = -MARCO_JUMP_SPEED_Y;
		}
		break;

	case MARCO_STATE_RELEASE_JUMP:
		if (vy < 0) vy += MARCO_JUMP_SPEED_Y / 2;
		break;

	case MARCO_STATE_SIT:
		if (isOnPlatform)
		{
			state = MARCO_STATE_IDLE;
			isSitting = true;
			vx = 0; vy = 0.0f;
			y += MARCO_SIT_HEIGHT_ADJUST;
		}
		break;

	case MARCO_STATE_SIT_RELEASE:
		if (isSitting)
		{
			isSitting = false;
			state = MARCO_STATE_IDLE;
			y -= MARCO_SIT_HEIGHT_ADJUST;
		}
		break;

	case MARCO_STATE_IDLE:
		ax = 0.0f;
		vx = 0.0f;
		break;

	case MARCO_STATE_SHOOTING:
		break;

	case MARCO_STATE_DIE:
		vy = -MARCO_JUMP_DEFLECT_SPEED;
		vx = 0;
		ax = 0;
		break;
	}

	CGameObject::SetState(state);
}

void CMARCO::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (isSitting)
	{
		left = x - MARCO_BIG_SITTING_BBOX_WIDTH / 2;
		top = y - MARCO_BIG_SITTING_BBOX_HEIGHT / 2;
		right = left + MARCO_BIG_SITTING_BBOX_WIDTH;
		bottom = top + MARCO_BIG_SITTING_BBOX_HEIGHT;
	}
	else
	{
		left = x - MARCO_BIG_BBOX_WIDTH / 2;
		top = y - MARCO_BIG_BBOX_HEIGHT / 2;
		right = left + MARCO_BIG_BBOX_WIDTH;
		bottom = top + MARCO_BIG_BBOX_HEIGHT;
	}
}


