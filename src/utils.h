#pragma once
#ifndef __UTILS__
#define __UTILS__

static int winsize_x = 1280, winsize_y = 720;
void loadTexture(const char* path, GLuint& t) {
	image* img = cg_load_image(path);

	// create textures
	glGenTextures(1, &t);
	glBindTexture(GL_TEXTURE_2D, t);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->width, img->height, 0, GL_RGB, GL_UNSIGNED_BYTE, img->ptr);
	// allocate and create mipmap
	int mip_levels = miplevels(winsize_x, winsize_y);
	for (int k = 1, w = img->width >> 1, h = img->height >> 1; k < mip_levels; k++, w = std::max(1, w >> 1), h = std::max(1, h >> 1))
		glTexImage2D(GL_TEXTURE_2D, k, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);


	glGenerateMipmap(GL_TEXTURE_2D);
	// configure texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// release the new image
	if (img) delete img;
}


std::vector<vertex> create_box_vertices() {
	std::vector<vertex> v;
	v.push_back({ vec3(-1, -1, 1), vec3(-1, -1, 1).normalize(), vec2(0.33f, 0.0f) });
	v.push_back({ vec3(1, -1, 1), vec3(1, -1, 1).normalize(), vec2(0.66f, 0.0f) });
	v.push_back({ vec3(-1, 1, 1), vec3(-1, 1, 1).normalize(), vec2(0.33f, 0.25f) });
	v.push_back({ vec3(1, 1, 1), vec3(1, 1, 1).normalize(), vec2(0.66f, 0.25f) });

	v.push_back({ vec3(-1, -1, -1), vec3(-1, -1, -1).normalize(), vec2(0.33f, 0.75f) });
	v.push_back({ vec3(1, -1, -1), vec3(1, -1, -1).normalize(), vec2(0.66f, 0.75f) });
	v.push_back({ vec3(-1, 1, -1), vec3(-1, 1, -1).normalize(), vec2(0.33f, 0.5f) });
	v.push_back({ vec3(1, 1, -1), vec3(1, 1, -1).normalize(), vec2(0.66f, 0.5f) });

	v.push_back({ vec3(-1, -1, 1), vec3(-1, -1, 1).normalize(), vec2(0.33f, 1.0f) });
	v.push_back({ vec3(1, -1, 1), vec3(1, -1, 1).normalize(), vec2(0.66f, 1.0f) });

	v.push_back({ vec3(-1, -1, 1), vec3(-1, -1, 1).normalize(), vec2(0.0f, 0.25f) });
	v.push_back({ vec3(-1, -1, -1), vec3(-1, -1, -1).normalize(), vec2(0.0f, 0.5f) });

	v.push_back({ vec3(1, -1, 1), vec3(1, -1, 1).normalize(), vec2(1.0f, 0.25f) });
	v.push_back({ vec3(1, -1, -1), vec3(1, -1, -1).normalize(), vec2(1.0f, 0.5f) });

	return v;
}

std::vector<vertex> create_skybox_vertices() {
	std::vector<vertex> v;
	v.push_back({ vec3(-1, -1, 1), vec3(-1, -1, 1).normalize(), vec2(0.0f,0.33f) });
	v.push_back({ vec3(1, -1, 1), vec3(1, -1, 1).normalize(), vec2(0.0f, 0.66f) });
	v.push_back({ vec3(-1, 1, 1), vec3(-1, 1, 1).normalize(), vec2(0.25f, 0.33f) });
	v.push_back({ vec3(1, 1, 1), vec3(1, 1, 1).normalize(), vec2(0.25f,0.66f) });

	v.push_back({ vec3(-1, -1, -1), vec3(-1, -1, -1).normalize(), vec2(0.75f,0.33f) });
	v.push_back({ vec3(1, -1, -1), vec3(1, -1, -1).normalize(), vec2(0.75f, 0.66f) });
	v.push_back({ vec3(-1, 1, -1), vec3(-1, 1, -1).normalize(), vec2(0.5f, 0.33f) });
	v.push_back({ vec3(1, 1, -1), vec3(1, 1, -1).normalize(), vec2(0.5f,0.66f) });

	v.push_back({ vec3(-1, -1, 1), vec3(-1, -1, 1).normalize(), vec2(1.0f, 0.33f) });
	v.push_back({ vec3(1, -1, 1), vec3(1, -1, 1).normalize(), vec2(1.0f, 0.66f) });

	v.push_back({ vec3(-1, -1, 1), vec3(-1, -1, 1).normalize(), vec2(0.25f, 0.0f) });
	v.push_back({ vec3(-1, -1, -1), vec3(-1, -1, -1).normalize(), vec2(0.5f, 0.0f) });

	v.push_back({ vec3(1, -1, 1), vec3(1, -1, 1).normalize(), vec2(0.25f, 1.0f) });
	v.push_back({ vec3(1, -1, -1), vec3(1, -1, -1).normalize(), vec2(0.5f, 1.0f) });

	return v;
}
void update_box_vertex_buffer(const std::vector<vertex> vertices, GLuint& vertex_array)
{
	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffers
	std::vector<uint> indices;
	//first roof
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(3);

	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(2);

	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(7);

	indices.push_back(2);
	indices.push_back(7);
	indices.push_back(6);

	indices.push_back(6);
	indices.push_back(7);
	indices.push_back(5);

	indices.push_back(6);
	indices.push_back(5);
	indices.push_back(4);

	indices.push_back(4);
	indices.push_back(5);
	indices.push_back(9);

	indices.push_back(4);
	indices.push_back(9);
	indices.push_back(8);

	indices.push_back(10);
	indices.push_back(2);
	indices.push_back(6);

	indices.push_back(10);
	indices.push_back(6);
	indices.push_back(11);

	indices.push_back(3);
	indices.push_back(12);
	indices.push_back(13);

	indices.push_back(3);
	indices.push_back(13);
	indices.push_back(7);
	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array) glDeleteVertexArrays(1, &vertex_array);
	vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}
void update_skybox_vertex_buffer(const std::vector<vertex> vertices, GLuint& vertex_array)
{
	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffers
	std::vector<uint> indices;
	//first roof
	indices.push_back(3);
	indices.push_back(1);
	indices.push_back(0);

	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(0);

	indices.push_back(7);
	indices.push_back(3);
	indices.push_back(2);

	indices.push_back(6);
	indices.push_back(7);
	indices.push_back(2);

	indices.push_back(5);
	indices.push_back(7);
	indices.push_back(6);

	indices.push_back(4);
	indices.push_back(5);
	indices.push_back(6);

	indices.push_back(9);
	indices.push_back(5);
	indices.push_back(4);

	indices.push_back(8);
	indices.push_back(9);
	indices.push_back(4);

	indices.push_back(6);
	indices.push_back(2);
	indices.push_back(10);

	indices.push_back(11);
	indices.push_back(6);
	indices.push_back(10);

	indices.push_back(13);
	indices.push_back(12);
	indices.push_back(3);

	indices.push_back(7);
	indices.push_back(13);
	indices.push_back(3);
	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array) glDeleteVertexArrays(1, &vertex_array);
	vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}
#endif