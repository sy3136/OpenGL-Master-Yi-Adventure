#pragma once
#ifndef __PARTICLE__
#define __PARTICLE__

class Particle {
public:
	Particle() {}
	Particle(vec3 pos, vec3 speed, float speed_scale, float scale, GLuint texture) {
		this->pos = pos;
		this->speed = speed;
		this->scale = scale;
		this->speed_scale = speed_scale;
		theta = float(rand() % 10) / 10.0f * PI;
		time_to_live = 2.0f;
		this->texture = texture;
	}

	vec3 pos, speed;
	float speed_scale, scale, theta;
	float time_to_live;
	mat4 model_matrix;

	GLuint texture;

	bool update(float delta_frame) {
		speed.z -= delta_frame * 7.0f;
		
		if (pos.z <= 0) {
			pos.z = 0;
			time_to_live -= delta_frame;
			if(time_to_live < 0) return true;
		}
		else pos += speed * delta_frame;

		mat4 adjust = mat4::translate(0, 0, -1.0f);
		mat4 rotate_mat = mat4::rotate(vec3(0, 1, 0), theta);
		mat4 scale_mat = mat4::scale(scale, scale, scale);
		mat4 trans_mat = mat4::translate(this->pos.x, this->pos.y, this->pos.z);

		model_matrix = trans_mat * scale_mat * rotate_mat * adjust;
		return false;
	}

	void render(GLuint program) {
		GLint uloc;
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(program, "mode"), 0);
		glUniform1i(glGetUniformLocation(program, "alphablending"), 1);
		glUniform1i(glGetUniformLocation(program, "bump"), 0);
		glUniform1i(glGetUniformLocation(program, "TEX"), 0);
		glUniform1i(glGetUniformLocation(program, "ALPHATEX"), 0);
		glUniform1f(glGetUniformLocation(program, "set_alpha"), time_to_live/2.0f);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
		glUniform1f(glGetUniformLocation(program, "set_alpha"), 0.0f);
	}
};

#endif