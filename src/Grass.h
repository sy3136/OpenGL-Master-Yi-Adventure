#pragma once
#ifndef __GRASS__
#define __GRASS__

const char* grass_texture[1] = {
	"../bin/textures/grass.bmp",
};
class Grass
{
public:
	Grass() {}
	Grass(vec3 pos, vec3 scale) {
		this->pos = pos;
		this->scale = scale;
		loadTexture(grass_texture[0], texture);
		printf("initial\n");
	}

	vec3 pos;
	vec3 scale;
	mat4 model_matrix;
	GLuint texture = 0;

	void update(float t) {
		mat4 wind_matrix =
		{
			1, 0, sin(t)*0.5f, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
		mat4 scale_matrix =
		{
			scale.x, 0, 0, 0,
			0, scale.y, 0, 0,
			0, 0, scale.z, 0,
			0, 0, 0, 1
		};
		mat4 transform_matrix =
		{
			1, 0, 0, this->pos.x,
			0, 1, 0, this->pos.y,
			0, 0, 1, this->pos.z,
			0, 0, 0, 1
		};
		
		model_matrix = transform_matrix * scale_matrix * wind_matrix;
	}

	void render(GLuint program, int mode) {
		GLint uloc;
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(program, "mode"), mode);
		glUniform1i(glGetUniformLocation(program, "alphablending"), 1);
		glUniform1i(glGetUniformLocation(program, "bump"), 0);
		glUniform1i(glGetUniformLocation(program, "TEX"), 0);
		glUniform1i(glGetUniformLocation(program, "ALPHATEX"), 0);

		glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, nullptr);
	}
};

#endif