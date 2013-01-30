#include "Player.h"
#include "HoverCraft.h"
#include "RaceTrack.h"
#include "Game.h"

Player::Player(RaceTrack* raceTrack) :
		raceTrack(raceTrack) {
}

const vec3& Player::getCraftPosition() {
	return craftPos;
}

const vec3& Player::getCraftDirection() {
	return craftDir;
}

const vec3& Player::getCraftUp() {
	return craftUp;
}

const vec2& Player::getCraftTrackOffset() {
	return carTrackPos;
}

const vec2& Player::getCraftTrackDirection() {
	return carTrackDirection;
}

void Player::init() {
	hoverCraft = new HoverCraft();
	hoverCraft->init();

	carTrackPos = vec2(0.0, 0.0);
	carTrackDirection = vec2(0.0, 1.0);
	carVelocity = 0.0;

	mSteeringRotation = 0.0;

	update();
}

void Player::setCarVelocity(float velocity) {
	carVelocity = velocity;
}

void Player::setSteeringRotation(float steer) {
	mSteeringRotation = steer;

	if (mSteeringRotation < -M_PI * 3 / 4)
		mSteeringRotation = -M_PI * 3 / 4;
	if (mSteeringRotation > M_PI * 3 / 4)
		mSteeringRotation = M_PI * 3 / 4;

	carTrackDirection = vec2(0, 1);
	carTrackDirection.rotate(-mSteeringRotation * 0.25);
}

void Player::update() {
	carTrackPos += (carTrackDirection * carVelocity) * Game::getSecondsPerFrame();

	vec3 craftSize = hoverCraft->getSize();
	float trackWidth = raceTrack->getWidth();

	if (carTrackPos.x < -trackWidth / 2.0 + craftSize.x / 2.0)
		carTrackPos.x = -trackWidth / 2.0 + craftSize.x / 2.0;
	if (carTrackPos.x > trackWidth / 2.0 - craftSize.x / 2.0)
		carTrackPos.x = trackWidth / 2.0 - craftSize.x / 2.0;

	raceTrack->getCameraVectors(carTrackPos.y, craftPos, craftDir, craftUp);

	craftPos += craftUp.cross(craftDir) * carTrackPos.x;

	craftDir.rotateAroundAxis(craftUp, -mSteeringRotation);
	craftUp.rotateAroundAxis(craftDir, mSteeringRotation);
}

void Player::render() {
	hoverCraft->setPosition(craftPos);
	hoverCraft->setDirection(craftDir);
	hoverCraft->setUp(craftUp);
	hoverCraft->render();
}

