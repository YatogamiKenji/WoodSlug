#pragma once

#include "GameObject.h"

class Portal : public CGameObject
{
	int scene_id;	// target scene to switch to 

	float width;
	float height;
public:
	Portal(float l, float t, float r, float b, int scene_id);
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);

	void RenderBoundingBox(void);

	int GetSceneId() { return scene_id; }
	int IsBlocking() { return 0; }
};

