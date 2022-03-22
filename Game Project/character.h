#pragma once
#include "GameObject.h"

#define character_WALKING_SPEED		0.1f 
//0.1f
#define character_JUMP_SPEED_Y		0.5f
#define character_JUMP_DEFLECT_SPEED 0.2f
#define character_GRAVITY			0.002f
#define character_DIE_DEFLECT_SPEED	 0.5f

#define character_STATE_IDLE			0
#define character_STATE_WALKING_RIGHT	100
#define character_STATE_WALKING_LEFT	200
#define character_STATE_JUMP			300
#define character_STATE_DIE				400
#define character_STATE_KNEEL			500

#define character_ANI_BIG_IDLE_RIGHT		0
#define character_ANI_BIG_IDLE_LEFT			1
#define character_ANI_SMALL_IDLE_RIGHT		2
#define character_ANI_SMALL_IDLE_LEFT		3

#define character_ANI_BIG_WALKING_RIGHT			4
#define character_ANI_BIG_WALKING_LEFT			5
#define character_ANI_SMALL_WALKING_RIGHT		6
#define character_ANI_SMALL_WALKING_LEFT		7
#define character_ANI_JUMP_LEFT		8
#define character_ANI_JUMP_RIGHT	9

#define character_ANI_DIE				10

#define	character_LEVEL_SMALL	1
#define	character_LEVEL_BIG		2

#define character_BIG_BBOX_WIDTH  30
#define character_BIG_BBOX_HEIGHT 40

#define character_SMALL_BBOX_WIDTH  13
#define character_SMALL_BBOX_HEIGHT 15

#define character_UNTOUCHABLE_TIME 5000


class Ccharacter : public CGameObject
{
	int level;
	int untouchable;
	DWORD untouchable_start;
public: 
	Ccharacter() : CGameObject()
	{
		level = character_LEVEL_BIG;
		untouchable = 0;
	}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *colliable_objects = NULL);
	virtual void Render();
	void SetState(int state);
	void SetLevel(int l) { level = l; }
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount(); }

	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
};