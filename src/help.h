#pragma once
#ifndef __HELP__
#define __HELP__
#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility


bool init_help(const char* image_path, const char* image_path_back, GLuint& SRC, GLuint& back, GLuint& vertex_array_image, GLuint& vertex_array_back_image) {
	// Image
	// create a frame buffer object for render-to-texture
	// create corners and vertices
	vertex corners[4];
	corners[0].pos = vec3(-1.5f, -0.8f, 0.0f);	corners[0].tex = vec2(0.0f, 0.0f);
	corners[1].pos = vec3(+1.5f, -0.8f, 0.0f);	corners[1].tex = vec2(1.0f, 0.0f);
	corners[2].pos = vec3(+1.5f, +0.8f, 0.0f);	corners[2].tex = vec2(1.0f, 1.0f);
	corners[3].pos = vec3(-1.5f, +0.8f, 0.0f);	corners[3].tex = vec2(0.0f, 1.0f);
	vertex vertices[6] = { corners[0], corners[1], corners[2], corners[0], corners[2], corners[3] };

	vertex back_corners[4];
	back_corners[0].pos = vec3(-1.0f, -1.0f, 0.0f);	back_corners[0].tex = vec2(0.0f, 0.0f);
	back_corners[1].pos = vec3(+1.0f, -1.0f, 0.0f);	back_corners[1].tex = vec2(1.0f, 0.0f);
	back_corners[2].pos = vec3(+1.0f, +1.0f, 0.0f);	back_corners[2].tex = vec2(1.0f, 1.0f);
	back_corners[3].pos = vec3(-1.0f, +1.0f, 0.0f);	back_corners[3].tex = vec2(0.0f, 1.0f);
	vertex back_vertices[6] = { back_corners[0], back_corners[1], back_corners[2], back_corners[0], back_corners[2], back_corners[3] };

	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array_image) glDeleteVertexArrays(1, &vertex_array_image);
	vertex_array_image = cg_create_vertex_array(vertex_buffer);
	if (!vertex_array_image) { printf("%s(): failed to create vertex aray\n", __func__); return false; }

	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(back_vertices), back_vertices, GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array_back_image) glDeleteVertexArrays(1, &vertex_array_back_image);
	vertex_array_back_image = cg_create_vertex_array(vertex_buffer);
	if (!vertex_array_back_image) { printf("%s(): failed to create vertex aray\n", __func__); return false; }

	// create a src texture (Lena texture)	
	SRC = create_texture(image_path, true); if (SRC == -1) return false;
	back = create_texture(image_path_back, true); if (SRC == -1) return false;
	return true;
}

void render_help(GLuint program, GLuint SRC, GLuint back, GLuint vertex_array_image, GLuint vertex_array_back_image, ivec2 window_size) {
	// separable Gaussian filtering
	glUseProgram(program);

	glUniform1i(glGetUniformLocation(program, "mode"), 2);
	glUniform1i(glGetUniformLocation(program, "is_title"), 1);
	glUniform1i(glGetUniformLocation(program, "TEX"), 0);
	glUniform1i(glGetUniformLocation(program, "is_back"), 1);

	glBindTexture(GL_TEXTURE_2D, back);
	glBindVertexArray(vertex_array_back_image);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glUniform1i(glGetUniformLocation(program, "is_back"), 0);


	glBindVertexArray(vertex_array_image);
	glUniform1i(glGetUniformLocation(program, "is_help"), 1);
	glBindTexture(GL_TEXTURE_2D, SRC);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	GLint uloc;
	uloc = glGetUniformLocation(program, "help_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, mat4::translate(vec3(0.02f, -0.02f, 0)));
	glUniform1i(glGetUniformLocation(program, "is_help"), 0);

	glBindTexture(GL_TEXTURE_2D, SRC);
	glDrawArrays(GL_TRIANGLES, 0, 6);


	float aspect = window_size.x / float(window_size.y);

	// keep 16:9
	float x = std::min(1 / aspect, 9 / 16.0f);
	float y = std::min(aspect / 16.0f * 9.0f, 1.0f);

	mat4 aspect_matrix =
	{
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	uloc = glGetUniformLocation(program, "aspect_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, aspect_matrix);

	glUniform1i(glGetUniformLocation(program, "is_title"), 0);
}

#endif