#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

struct circle_t
{
	float index;
	vec3	center=vec3(0);		// 2D position for translation
	float	radius=1.0f;		// radius
	float	self_rotate_speed;
	float	rotate_speed;
	vec4	color;				// RGBA color in [0,1]
	mat4	model_matrix;		// modeling transformation

	//***추가
	GLuint texture = 0;
	GLuint alpha_texture = 0;
	GLuint bump_texture = 0;
	std::vector<circle_t> satellite;
	vec3	standardPos = vec3(0);

	// public functions
	void	update( float t );	
};

inline std::vector<circle_t> create_circles()
{
	std::vector<circle_t> circles;
	circle_t moon = { 0.5f, vec3(0, 0, 0), 0.15f, 1.0f, 1.0f, vec4(1.0f, 0.5f, 0.5f, 1.0f) };
	circle_t ring = { 0.0f, vec3(0, 0, 0), 0.6f, 1.0f, 1.0f, vec4(1.0f, 0.5f, 0.5f, 1.0f) };
	circle_t sat[4];
	sat[0] = { 0.5f, vec3(0, 0, 0), 0.1f, 1.0f, 3.0f, vec4(1.0f, 0.5f, 0.5f, 1.0f) };
	sat[1] = { 0.6f, vec3(0, 0, 0), 0.15f, 1.0f, 2.0f, vec4(1.0f, 0.5f, 0.5f, 1.0f) };
	sat[2] = { 0.7f, vec3(0, 0, 0), 0.15f, 1.0f, 1.0f, vec4(1.0f, 0.5f, 0.5f, 1.0f) };
	sat[3] = { 1.0f, vec3(0, 0, 0), 0.2f, 1.0f, 0.9f, vec4(1.0f, 0.5f, 0.5f, 1.0f) };

	circle_t c[9];
	c[0] = {0, vec3(0,0,0),1.0f, 0.1f, 0.0f,vec4(1.0f,0.5f,0.5f,1.0f)}; // 태양
	c[1] = { 1, vec3(2,0,0),0.2f, 0.1f, 4.0f,vec4(1.0f,0.5f,0.5f,1.0f) }; // 수성
	c[2] = { 2, vec3(4,0,0),0.3f, -0.1f, 3.5f,vec4(1.0f,0.5f,0.5f,1.0f) }; // 금성
	c[3] = { 3, vec3(6,0,0),0.3f, 1.0f, 3.0f,vec4(1.0f,0.5f,0.5f,1.0f) }; // 지구
	c[3].satellite.push_back(moon); //달
	c[4] = { 4, vec3(8,0,0),0.25f, 1.0f, 2.5f,vec4(1.0f,0.5f,0.5f,1.0f) }; // 화성
	c[5] = { 5, vec3(10,0,0),0.7f, 2.0f, 1.3f,vec4(1.0f,0.5f,0.5f,1.0f) }; // 목성
	for (int i = 0; i < 4; i++) c[5].satellite.push_back(sat[i]);
	c[6] = { 7, vec3(12,0,0),0.6f, 2.1f, 0.9f,vec4(1.0f,0.5f,0.5f,1.0f) }; // 토성
	c[6].satellite.push_back(ring);
	c[7] = { 9, vec3(14,0,0),0.5f, -1.5f, 0.68f,vec4(1.0f,0.5f,0.5f,1.0f) }; // 천왕성
	ring.radius = 0.5f;
	c[7].satellite.push_back(ring);
	for (int i = 0; i < 3; i++) c[7].satellite.push_back(sat[i]);
	c[8] = { 10, vec3(16,0,0),0.5f, 1.5f, 0.54f,vec4(1.0f,0.5f,0.5f,1.0f) }; // 혜왕성
	for (int i = 0; i < 3; i++) c[8].satellite.push_back(sat[i]);
	for (int i = 0; i < 9; i++) circles.emplace_back(c[i]);

	return circles;
}

inline void circle_t::update( float t )
{
	float c	= cos(t*self_rotate_speed), s=sin(t* self_rotate_speed);
	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, radius, 0,
		0, 0, 0, 1
	};

	mat4 rotation_matrix =
	{
		c,-s, 0, 0,
		s, c, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	if (index) {
		center.x = index * 2.0f * cos(t * 0.5f * rotate_speed / (float)index);
		center.y = index * 2.0f * sin(t * 0.5f * rotate_speed / (float)index);
		center += standardPos;
	}
	else center = standardPos;
	mat4 translate_matrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, center.z,
		0, 0, 0, 1
	};
	for (auto& c : satellite) c.standardPos = center;
	model_matrix = translate_matrix*rotation_matrix*scale_matrix;
}

#endif
