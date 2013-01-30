#ifndef _SKY_BOX_H__
#define _SKY_BOX_H__

#include "vec.h"
#include <MAUtil/Vector.h>
#include <GLES/gl.h>
#include "OpenGLHelpers.h"

class Skybox : public TriangleStripVBO {
public:
	void init(GLuint skymap);
	void render();

	void setDirection(const vec3& dir);
	void setUp(const vec3& up);

private:
	vec3 mDirection;
	vec3 mUp;
	GLuint mSkymap;
};


#endif
