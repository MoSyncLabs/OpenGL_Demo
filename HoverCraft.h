#ifndef _HOVER_CRAFT_H_
#define _HOVER_CRAFT_H_

#include "vec.h"
#include <GLES/gl.h>

class HoverCraft
{
public:
	HoverCraft();

	void init();
	void render();
	void setPosition(const vec3& pos);
	void setDirection(const vec3& dir);
	void setUp(const vec3& up);

	vec3 getSize();

private:
	vec3 mPosition;
	vec3 mRotation;
	vec3 mDirection;
	vec3 mUp;

	GLuint mVBO;
	GLuint mIBO;
	int mNumIndecies;

	vec3 mSize;
};

#endif
