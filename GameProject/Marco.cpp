#include <algorithm>
#include "debug.h"

#include "Marco.h"
#include "Game.h"

#include "Collision.h"

void CMARCO::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	x += vx * dt;

	if (abs(vx) > abs(maxVx)) vx = maxVx;

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

	//if (dynamic_cast<CGoomba*>(e->obj))
	//	OnCollisionWithGoomba(e);
	//else if (dynamic_cast<CCoin*>(e->obj))
	//	OnCollisionWithCoin(e);
	//else if (dynamic_cast<CPortal*>(e->obj))
	//	OnCollisionWithPortal(e);
}

//void CMARCO::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
//{
//	CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
//
//	// jump on top >> kill Goomba and deflect a bit 
//	if (e->ny < 0)
//	{
//		if (goomba->GetState() != GOOMBA_STATE_DIE)
//		{
//			goomba->SetState(GOOMBA_STATE_DIE);
//			vy = -MARCO_JUMP_DEFLECT_SPEED;
//		}
//	}
//	else // hit by Goomba
//	{
//		if (untouchable == 0)
//		{
//			if (goomba->GetState() != GOOMBA_STATE_DIE)
//			{
//				if (level > MARCO_LEVEL_SMALL)
//				{
//					level = MARCO_LEVEL_SMALL;
//					StartUntouchable();
//				}
//				else
//				{
//					DebugOut(L">>> MARCO DIE >>> \n");
//					SetState(MARCO_STATE_DIE);
//				}
//			}
//		}
//	}
//}
//
//void CMARCO::OnCollisionWithCoin(LPCOLLISIONEVENT e)
//{
//	e->obj->Delete();
//	coin++;
//}
//
//void CMARCO::OnCollisionWithPortal(LPCOLLISIONEVENT e)
//{
//	CPortal* p = (CPortal*)e->obj;
//	CGame::GetInstance()->InitiateSwitchScene(p->GetSceneId());
//}


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
	aniId = GetAniId();

	animations->Get(aniId)->Render(x, y);

	//RenderBoundingBox();

	DebugOutTitle(L"Coins: %d", coin);
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


