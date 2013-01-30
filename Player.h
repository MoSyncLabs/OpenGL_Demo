#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "vec.h"

class RaceTrack;
class HoverCraft;

class Player
{
public:
	Player(RaceTrack* raceTrack);
	const vec3& getCraftPosition();
	const vec3& getCraftDirection();
	const vec3& getCraftUp();
	const vec2& getCraftTrackOffset();
	const vec2& getCraftTrackDirection();
	void init();
	void setCarVelocity(float velocity);
	void setSteeringRotation(float steer);
	void update();
	void render();
private:
	vec3 craftPos, craftDir, craftUp;

	float mSteeringRotation;
	RaceTrack* raceTrack;
	HoverCraft* hoverCraft;
	vec2 carTrackPos;
	vec2 carTrackDirection;
	float carVelocity;

};

#endif // _PLAYER_H_
