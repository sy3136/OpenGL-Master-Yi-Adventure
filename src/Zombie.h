#pragma once
#ifndef __ZOMBIE__
#define __ZOMBIE__
const char* zombie_texture[4] = {
	"../bin/textures/zombie_head2.bmp",
	"../bin/textures/zombie_body.bmp",
	"../bin/textures/zombie_body.bmp",
	"../bin/textures/zombie_body.bmp"
};
class Zombie : public Character
{
public:
	Zombie() {}

	Zombie(vec3 pos, float scale) {
		arm1 = Box(zombie_texture[1], 1.0f, 1.0f, 2.0f, 0.25f, vec3(0.0f, -0.75f, 1.5f + 0.5f), 1.0f, PI/2, 15.0f);
		arm2 = Box(zombie_texture[1], 1.0f, 1.0f, 2.0f, 0.25f, vec3(0.0f, 0.75f, 1.5f + 0.5f), 1.0f, PI/2, 15.0f);
		body = Box(zombie_texture[1], 1.0f, 2.0f, 2.0f, 0.25f, vec3(0.0f, 0.0f, 1.5f + 0.5f), 0.0f);
		leg1 = Box(zombie_texture[1], 1.0f, 1.0f, 2.0f, 0.25f, vec3(0.0f, -0.25f, 0.5f + 0.5f), -1.0f);
		leg2 = Box(zombie_texture[1], 1.0f, 1.0f, 2.0f, 0.25f, vec3(0.0f, 0.25f, 0.5f + 0.5f), 1.0f);
		head = Box(zombie_texture[0], 2.0f, 2.0f, 2.0f, 0.25f, vec3(0.0f, 0.0f, 2.25f + 0.5f), 0.0f);
		this->pos = pos;
		this->scale = scale;
		this->speed_scale = 3.0f;
		this->accel_scale = 3.0f;
		x_moving = y_moving = attacking = false;
		speed_theta = 0;
	}

	bool attacking = false;
	bool knockbacking = false;

	void start_attack(float t) {
		arm1.start_rotate(t);
		arm2.start_rotate(t);
	}

	void knockback(vec3 force, float scale) {
		speed.x = force.x * scale;
		speed.y = force.y * scale;
		knockbacking = true;
		x_moving = y_moving = false;
	}

	void update(float t, float delta_frame, vec3 playerPos) {
		if (!knockbacking) {
			if ((playerPos - pos).length() > 1.5f) {
				float theta = atan2(playerPos.y - pos.y, playerPos.x - pos.x);
				if (speed.length() < 1.0f) {
					speed.x += cos(theta) * delta_frame * accel_scale;
					speed.y += sin(theta) * delta_frame * accel_scale;
				}
				else {
					speed.x += cos(theta) * delta_frame * accel_scale * 2.0f;
					speed.y += sin(theta) * delta_frame * accel_scale * 2.0f;
					speed = speed.normalize();
				}
				x_moving = y_moving = true;
				attacking = false;
				arm1.end_rotate();
				arm2.end_rotate();
			}
			else {
				x_moving = y_moving = false;
				attacking = true;
				start_attack(t);
			}
		}

		pos.x += speed.x * delta_frame * speed_scale;
		pos.y += speed.y * delta_frame * speed_scale;
		if (!x_moving && speed.x) {
			float origin_x = speed.x;
			speed.x = speed.x + ((speed.x > 0) ? -accel_scale : accel_scale) * delta_frame * speed_scale * (abs(speed.x) / (abs(speed.x) + abs(speed.y)));
			if ((origin_x > 0) != (speed.x > 0)) speed.x = 0;
		}
		if (!y_moving && speed.y) {
			float origin_y = speed.y;
			speed.y = speed.y + ((speed.y > 0) ? -accel_scale : accel_scale) * delta_frame * speed_scale * (abs(speed.y) / (abs(speed.x) + abs(speed.y)));
			if ((origin_y > 0) != (speed.y > 0)) speed.y = 0;
		}
		if (speed.x || speed.y) {
			speed_theta = atan2(speed.y, speed.x);
			leg1.start_rotate(t);
			leg2.start_rotate(t);
		}
		else {
			knockbacking = false;
			leg1.end_rotate();
			leg2.end_rotate();
		}
		arm1.update(t, pos, speed_theta);
		arm2.update(t, pos, speed_theta);
		leg1.update(t, pos, speed_theta);
		leg2.update(t, pos, speed_theta);
		body.update(t, pos, speed_theta);
		head.update(t, pos, speed_theta);
	}

	void render(GLuint program) {
		arm1.render(program, 0);
		arm2.render(program, 0);
		leg1.render(program, 0);
		leg2.render(program, 0);
		body.render(program, 0);
		head.render(program, 0);
	}
};

#endif