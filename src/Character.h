#pragma once
#ifndef __CHARACTER__
#define __CHARACTER__
const char* character_texture[4] = {
	"../bin/textures/ch_head.bmp",
	"../bin/textures/ch_body.bmp",
	"../bin/textures/ch_body.bmp",
	"../bin/textures/ch_body.bmp"
};
class Character
{
public:
	Character() {}
	Character(vec3 pos, float scale) {
		weapon_speed = 10.0f;
		arm1 = Box(character_texture[2], 1.0f, 1.0f, 2.0f, 0.25f, vec3(0.0f, -0.75f, 1.5f+0.5f), 1.0f);
		arm2 = Box(character_texture[2], 1.0f, 1.0f, 2.0f, 0.25f, vec3(0.0f, 0.75f, 1.5f + 0.5f), 1.0f, PI/4.0f, weapon_speed, 0.0f, PI);
		body = Box(character_texture[1], 1.0f, 2.0f, 2.0f, 0.25f, vec3(0.0f, 0.0f, 1.5f + 0.5f), 0.0f);
		leg1 = Box(character_texture[3], 1.0f, 1.0f, 2.0f, 0.25f, vec3(0.0f, -0.25f, 0.5f + 0.5f), -1.0f);
		leg2 = Box(character_texture[3], 1.0f, 1.0f, 2.0f, 0.25f, vec3(0.0f, 0.25f, 0.5f + 0.5f), 1.0f);
		head = Box(character_texture[0], 2.0f, 2.0f, 2.0f, 0.25f, vec3(0.0f, 0.0f, 2.25f + 0.5f), 0.0f);
		weapon = Weapon(character_texture[3], vec3(0.0f, 0.75f, 1.5f + 0.5f), 1.0f, weapon_speed, PI);
		this->pos = pos;
		this->scale = scale;
		this->speed_scale = 8.0f;
		this->accel_scale = 0.5f;
		x_moving = y_moving = false;
		speed_theta = 0;
	}
	Box arm1, arm2, leg1, leg2, body, head;
	Weapon weapon;
	vec3 pos;
	float scale, speed_scale, accel_scale;
	vec2 speed;
	float speed_theta;
	float weapon_speed;

	bool x_moving, y_moving;

	vec3 getAttackingPos() { return pos + vec3(cos(speed_theta), sin(speed_theta), 0); }
	vec3 getPos() { return pos; }

	void move(float t, float x_dir, float y_dir) {
		start_moving(t);
		if (x_dir) { x_moving = true; speed.x = x_dir; }
		if (y_dir) { y_moving = true; speed.y = y_dir; }
	}

	void stop(float x_dir, float y_dir) {
		if (x_dir && ((speed.x > 0) == (x_dir > 0))) x_moving = false;
		if (y_dir && ((speed.y > 0) == (y_dir > 0))) y_moving = false;
	}

	void attack(float t) {
		arm2.start_rotate(t);
		weapon.stick.start_rotate(t);
		arm2.end_rotate();
		weapon.stick.end_rotate();
	}

	bool isAttacking(float t) {
		return weapon.stick.isRotating(t);
	}

	void start_moving(float t) {
		arm1.start_rotate(t);
		
		leg1.start_rotate(t);
		leg2.start_rotate(t);
		
	}

	void end_moving() {
		x_moving = y_moving = false;
		arm1.end_rotate();
		leg1.end_rotate();
		leg2.end_rotate();
	}

	void update(float t, float delta_frame) {
		pos.x += speed.x * delta_frame * speed_scale;
		pos.y += speed.y * delta_frame * speed_scale;
		if (!x_moving && speed.x) {
			float origin_x = speed.x;
			speed.x = speed.x + ((speed.x > 0) ? -accel_scale : accel_scale) * delta_frame * speed_scale;
			if ((origin_x > 0) != (speed.x > 0)) speed.x = 0;
		} else if (speed.x) speed /= sqrtf(speed.x * speed.x + speed.y * speed.y);
		if (!y_moving && speed.y) {
			float origin_y = speed.y;
			speed.y = speed.y + ((speed.y > 0) ? -accel_scale : accel_scale) * delta_frame * speed_scale;
			if ((origin_y > 0) != (speed.y > 0)) speed.y = 0;
		} else if (speed.y) speed /= sqrtf(speed.x * speed.x + speed.y * speed.y);
		if (speed.x || speed.y) {
			speed_theta = atan2(speed.y, speed.x);
		}
		arm1.update(t, pos, speed_theta);
		arm2.update(t, pos, speed_theta);
		leg1.update(t, pos, speed_theta);
		leg2.update(t, pos, speed_theta);
		body.update(t, pos, speed_theta);
		head.update(t, pos, speed_theta);
		weapon.update(t, pos, speed_theta);
	}

	void render(GLuint program) {
		arm1.render(program, 0);
		arm2.render(program, 0);
		leg1.render(program, 0);
		leg2.render(program, 0);
		body.render(program, 0);
		head.render(program, 0);
		weapon.render(program, 0);
	}
};

#endif