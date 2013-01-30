#include "Terrain.h"
//#include <stdlib.h>
//#include "System.h"
#include <mastdlib.h>

using namespace MAUtil;

void Terrain::init(MAHandle heightMap) {

	int imageWidth = EXTENT_X(maGetImageSize(heightMap));
	int imageHeight = EXTENT_Y(maGetImageSize(heightMap));

	int *data = new int[imageWidth*imageHeight];

	mHeightMap = heightMap;

	MARect srcRect = {0,0,imageWidth,imageHeight};
	maGetImageData(heightMap, data, &srcRect, imageWidth);

	MAUtil::Vector<TriangleStrip> strips;

	for(int j = 0; j < imageHeight-1; j++)
	{
		strips.add(TriangleStrip());
		TriangleStrip& strip = strips[strips.size()-1];
		for(int i = 0; i < imageWidth; i++)
		{
			float h1 = (float)(data[i+j*imageWidth]>>24)/255.0;
			float h2 = (float)(data[i+(j+1)*imageWidth]>>24)/255.0;

			float u = (float)i/(float)imageWidth;
			float v1 = (float)j/(float)imageHeight;
			float v2 = (float)(j+1)/(float)imageHeight;

			vec3 pos_a = vec3(u, h1, v1);
			vec3 pos_b = vec3(u, h2, v2);

			strip.vertices.add(pos_a);
			strip.vertices.add(pos_b);

			strip.textureCoordinates.add(vec2(u, v1));
			strip.textureCoordinates.add(vec2(u, v2));
		}
	}

	delete data;

	initVBO(strips);
}


void Terrain::render() {

	renderVBO();
}
