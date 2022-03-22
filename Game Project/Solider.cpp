#include "Solider.h"

void CSolider::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	left = x;
	top = y;
	right = x + Solider_BBOX_WIDTH;

	if (state == Solider_STATE_DIE)
		bottom = y + Solider_BBOX_HEIGHT_DIE;
	else 	
		bottom = y + Solider_BBOX_HEIGHT;
}

void CSolider::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	CGameObject::Update(dt, coObjects);

	//
	// TO-DO: make sure Solider can interact with the world and to each of them too!
	// 

	x += dx;
	y += dy;

	if (vx < 0 && x < 0) {
		x = 0; vx = -vx;
		state = Solider_STATE_WALKING_RIGHT;
	}

	if (vx > 0 && x > 290) {
		state = Solider_STATE_WALKING_LEFT;
		x = 290; vx = -vx;
	}
}

void CSolider::Render()
{
	int ani;
	if (state == Solider_STATE_DIE) {
		ani = Solider_ANI_DIE;
	}
	else
	{
		if (vx > 0) ani = Solider_ANI_WALKING_RIGHT;
		else ani = Solider_ANI_WALKING_LEFT;
	}
	animations[ani]->Render(x,y);
	RenderBoundingBox();
}

void CSolider::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
		case Solider_STATE_DIE:
			y += Solider_BBOX_HEIGHT - Solider_BBOX_HEIGHT_DIE +1;
			vx = 0;
			vy = 0;
			break;
		case Solider_STATE_WALKING_RIGHT:
			vx = Solider_WALKING_SPEED;
			nx = 1;
			break;
		case Solider_STATE_WALKING_LEFT:
			vx = -Solider_WALKING_SPEED;
			nx = -1;
			break;
	}
}
