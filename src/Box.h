#pragma once
#ifndef __BOX__
#define __BOX__

class Box
{
public:
	Box() {}
	Box(const char* texture_path, float w, float h, float z, float scale, vec3 pos,
		float rotating_dir=0.0f, float thetaxz = 0.0f, float rotating_speed = 8.0f, vec3 weapon_pos_factor = vec3(0, 0, 0), float rotating_range = PI/4.0f) {
		loadTexture(texture_path, texture);
		this->w = w*scale;
		this->h = h*scale;
		this->z = z*scale;
		this->pos = pos;
		this->scale = scale;
		this->rotating_dir = rotating_dir;
		this->rotating_speed = rotating_speed;
		this->thetaxz = thetaxz;
		this->weapon_pos_factor = weapon_pos_factor;
		this->rotating_range = rotating_range;
	}

	vec3	pos;
	float	scale;
	mat4	model_matrix;
	float	w;
	float	h;
	float	z;
	GLuint texture = 0;
	GLuint alpha_texture = 0;
	GLuint bump_texture = 0;

	float	start_rotate_time;
	float	end_rotate_time = 0;
	float	rotating_dir = 0;
	float	rotating_speed = 0;
	float	rotating_range = 0;
	vec3	weapon_pos_factor = vec3(0, 0, 0);
	bool	rotating = false;

	float thetaxz;

	void start_rotate(float t) {
		if (!rotating && end_rotate_time < t) {
			start_rotate_time = t;
			end_rotate_time = t + PI / rotating_speed;
		}
		rotating = true;
	}

	void end_rotate() {
		rotating = false;
	}

	bool isRotating(float t) {
		return (t < end_rotate_time);
	}

	void update(float t, vec3 pos, float theta) {
		float origin_theta_c = cos(thetaxz), origin_theta_s = sin(thetaxz);
		mat4 rotation_matrix =
		{
			origin_theta_c, 0, -origin_theta_s, 0,
			0, 1, 0, 0,
			origin_theta_s, 0, origin_theta_c, 0,
			0, 0, 0, 1
		};

		mat4 translate_center =
		{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, -1.0f,
			0, 0, 0, 1
		};
		mat4 weapon_matrix =
		{
			1, 0, 0, weapon_pos_factor.x,
			0, 1, 0, weapon_pos_factor.y,
			0, 0, 1, weapon_pos_factor.z,
			0, 0, 0, 1
		};
		if (t >= end_rotate_time && rotating) end_rotate_time += PI / rotating_speed;
		if (t < end_rotate_time) {
			float theta = t - start_rotate_time;
			float curT = sinf(theta * rotating_speed) * rotating_range * rotating_dir;
			float c = cos(thetaxz+curT), s = sin(thetaxz+curT);

			rotation_matrix =
			{
				c, 0, -s, 0,
				0, 1, 0, 0,
				s, 0, c, 0,
				0, 0, 0, 1
			};

			translate_center =
			{
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, -1.0f,
				0, 0, 0, 1
			};
		}
		else if (rotating) rotating = false;

		mat4 scale_matrix =
		{
			w, 0, 0, 0,
			0, h, 0, 0,
			0, 0, z, 0,
			0, 0, 0, 1
		};

		mat4 my_translate_matrix =
		{
			1, 0, 0, this->pos.x,
			0, 1, 0, this->pos.y,
			0, 0, 1, this->pos.z,
			0, 0, 0, 1
		};
		float c = cos(theta), s = sin(theta);
		mat4 parent_rotation_matrix =
		{
			c, -s, 0, 0,
			s, c, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
		mat4 parent_translate_matrix =
		{
			1, 0, 0, pos.x,
			0, 1, 0, pos.y,
			0, 0, 1, pos.z,
			0, 0, 0, 1
		};
		model_matrix = parent_translate_matrix * parent_rotation_matrix * my_translate_matrix * rotation_matrix * weapon_matrix * scale_matrix * translate_center;
	}

	void render(GLuint program, int mode) {
		
		GLint uloc;
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(program, "mode"), mode);
		glUniform1i(glGetUniformLocation(program, "alphablending"), 0);
		glUniform1i(glGetUniformLocation(program, "bump"), 0);
		glUniform1i(glGetUniformLocation(program, "TEX"), 0);
		
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
	}
};

#endif