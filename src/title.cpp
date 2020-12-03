#include "cgmath.h"	// slee's simple math library
#include "cgut.h"	// slee's OpenGL utility

GLuint create_texture(const char* image_path, bool b_mipmap)
{
	// load the image with vertical flipping
	image* img = cg_load_image(image_path); if (!img) return -1;
	int w = img->width, h = img->height;

	// create a src texture (lena texture)
	GLuint texture; glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img->ptr);
	if (img) delete img; // release image

	// build mipmap
	if (b_mipmap && glGenerateMipmap)
	{
		int mip_levels = 0; for (int k = std::max(w, h); k; k >>= 1) mip_levels++;
		for (int l = 1; l < mip_levels; l++)
			glTexImage2D(GL_TEXTURE_2D, l, GL_RGB8, std::max(1, w >> l), std::max(1, h >> l), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	// set up texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, b_mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);

	return texture;
}
bool init_title() {
	// Image
	// create a frame buffer object for render-to-texture
	// create corners and vertices
	vertex corners[4];
	corners[0].pos = vec3(-1.0f, -1.0f, 0.0f);	corners[0].tex = vec2(0.0f, 0.0f);
	corners[1].pos = vec3(+1.0f, -1.0f, 0.0f);	corners[1].tex = vec2(1.0f, 0.0f);
	corners[2].pos = vec3(+1.0f, +1.0f, 0.0f);	corners[2].tex = vec2(1.0f, 1.0f);
	corners[3].pos = vec3(-1.0f, +1.0f, 0.0f);	corners[3].tex = vec2(0.0f, 1.0f);
	vertex vertices[6] = { corners[0], corners[1], corners[2], corners[0], corners[2], corners[3] };
	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array_image) glDeleteVertexArrays(1, &vertex_array_image);
	vertex_array_image = cg_create_vertex_array(vertex_buffer);
	if (!vertex_array_image) { printf("%s(): failed to create vertex aray\n", __func__); return false; }


	// create a src texture (Lena texture)	
	SRC = create_texture(image_path, true); if (SRC == -1) return false;
	return true;
}

void render_title() {
	// separable Gaussian filtering
	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "is_image"), 1);
	glBindTexture(GL_TEXTURE_2D, SRC);
	glUniform1i(glGetUniformLocation(program, "TEX"), 0);
	// bind vertex array object
	glBindVertexArray(vertex_array_image);

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
	GLint uloc;
	uloc = glGetUniformLocation(program, "aspect_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, aspect_matrix);
	// render quad vertices
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glUniform1i(glGetUniformLocation(program, "is_image"), 0);
}
