#pragma once
#ifndef __SWORD__
#define __SWORD__
class Sword
{
public:
	Sword() {}
	Sword(mesh2* pMesh, vec3 scale, vec3 pos, float rotating_dir, float rotating_speed = 8.0f, float rotating_range = PI / 4.0f, int is_hand = 0) {
		this->pMesh = pMesh;
		this->scale = scale;
		this->pos = pos;
		this->rotating_dir = rotating_dir;
		this->rotating_speed = rotating_speed;
		this->rotating_range = rotating_range;
		power = 1;
	}

	vec3 scale;
	vec3 pos;
	int power;
	mesh2* pMesh;
	mat4 model_matrix;
	vec3 weapon_pos_factor = vec3(-1.0f, 0.0f, 1.0f);

	float thetaxz = 5*PI/4;


	float	start_rotate_time;
	float	end_rotate_time = 0;
	float	rotating_dir = 1.0f;
	float	rotating_speed = 8.0f;
	float	rotating_range = PI / 4.0f;
	bool	rotating = false;

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


	void update(GLuint program, float t, vec3 pos, float theta) {
		float origin_theta_c = cos(thetaxz), origin_theta_s = sin(thetaxz);
		mat4 weapon_rotation_matrix = mat4::rotate(vec3(0, 0, 1), PI / 2.0f);

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
			float c = cos(thetaxz + curT), s = sin(thetaxz + curT);

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

		mat4 scale_matrix = mat4::scale(scale);

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
		model_matrix = parent_translate_matrix * parent_rotation_matrix * my_translate_matrix * rotation_matrix * weapon_matrix * weapon_rotation_matrix * scale_matrix;
	}

	void render(GLuint program, int mode) {

		for (size_t k = 0, kn = pMesh->geometry_list.size(); k < kn; k++) {

			glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_TRUE, model_matrix);
			geometry& g = pMesh->geometry_list[k];
			glUniform1i(glGetUniformLocation(program, "is_sword"), 1);
			glUniform1i(glGetUniformLocation(program, "is_model"), 1);
			glUniformMatrix4fv(glGetUniformLocation(program, "sword_matrix"), 1, GL_TRUE, model_matrix);
			glUniform1i(glGetUniformLocation(program, "alphablending"), 0);
			glUniform1i(glGetUniformLocation(program, "bump"), 0);
			glUniform1i(glGetUniformLocation(program, "mode"), 0);
			if (g.mat->textures.diffuse) {
				glBindTexture(GL_TEXTURE_2D, g.mat->textures.diffuse->id);
				glUniform1i(glGetUniformLocation(program, "TEX"), 0);	 // GL_TEXTURE0
				glUniform1i(glGetUniformLocation(program, "use_texture"), true);
			}

			else {
				glUniform4fv(glGetUniformLocation(program, "diffuse"), 1, (const float*)(&g.mat->diffuse));
				glUniform1i(glGetUniformLocation(program, "use_texture"), false);
			}

			// render vertices: trigger shader programs to process vertex data
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMesh->index_buffer);
			glDrawElements(GL_TRIANGLES, g.index_count, GL_UNSIGNED_INT, (GLvoid*)(g.index_start * sizeof(GLuint)));
		}
		glUniform1i(glGetUniformLocation(program, "is_model"), 0);
		glUniform1i(glGetUniformLocation(program, "is_sword"), 0);

	}

};

#endif