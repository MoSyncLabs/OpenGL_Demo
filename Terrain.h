#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include "vec.h"
#include <MAUtil/Vector.h>
#include <GLES/gl.h>
#include "OpenGLHelpers.h"

class Terrain : public TriangleStripVBO {
public:
	void init(MAHandle heightMap);
	void render();

private:
	MAHandle mHeightMap;
};


#endif
