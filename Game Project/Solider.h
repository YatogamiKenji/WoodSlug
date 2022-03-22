#pragma once
#include "GameObject.h"

#define Solider_WALKING_SPEED 0.05f;

#define Solider_BBOX_WIDTH 40
#define Solider_BBOX_HEIGHT 40
#define Solider_BBOX_HEIGHT_DIE 20

#define Solider_STATE_WALKING 400
#define Solider_STATE_DIE 300
#define Solider_STATE_WALKING_RIGHT 100
#define Solider_STATE_WALKING_LEFT 200

#define Solider_ANI_WALKING 2
#define Solider_ANI_DIE 3
#define Solider_ANI_WALKING_RIGHT 0
#define Solider_ANI_WALKING_LEFT 1

class CSolider : public CGameObject
{
	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects);
	virtual void Render();


public: 	
	virtual void SetState(int state);
};