#pragma once
#ifndef __TREE_
#define __TREE_

const char* tree_texture[2] = {
	"../bin/textures/tree.bmp",
	"../bin/textures/tree_grass.bmp"
};

class Tree
{
public:
	Tree() {}
	Tree(vec3 pos, float scale) {
		this->pos = pos;
		this->scale = scale;
		body = Box(tree_texture[0], 1.0f, 1.0f, 4.0f, 0.5f, vec3(0, 0, 4.0f));
		grass = Box(tree_texture[0])
	}

	vec3 pos;
	float scale;

	Box body;
	Box grass;
};

#endif