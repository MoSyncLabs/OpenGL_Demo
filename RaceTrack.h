#ifndef _RACE_TRACK_H_
#define _RACE_TRACK_H_

#include "vec.h"
#include <MAUtil/Vector.h>
#include <GLES/gl.h>
#include "OpenGLHelpers.h"

// should derive from SceneObject or something.
class RaceTrack : public TriangleStripVBO {
public:
	void init(int numRaceTrackNodes);
	void render();

	// t = 0-1
	void getCameraVectors(float t, vec3& position, vec3& direction, vec3& up);

	void setFloorTexture(GLuint floorTexture);
	float getWidth();

	void renderMap(const vec3 &pos, const vec3& dir);

private:

	GLuint floorTexture;
	MAUtil::Vector<vec3> leftPoints;
	MAUtil::Vector<vec3> rightPoints;

	float mTrackWidth;
	float mTotalLength;
};


#endif
