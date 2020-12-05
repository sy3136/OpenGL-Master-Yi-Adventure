// vertex attributes
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texcoord;

// outputs of vertex shader = input to fragment shader
out vec4 epos;	// eye-space position
out vec3 norm;	// per-vertex normal before interpolation
out vec2 tc;	// texture coordinate
out vec3 norm_origin;
out mat3 matrix;

// matrices
uniform mat4 model_matrix;
uniform mat4 sword_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform mat4 aspect_matrix;

// text
uniform mat4 text_matrix;
uniform int is_text;
uniform int is_back;
uniform int is_title;
uniform int is_sword;

void main()
{
	tc = texcoord;

	vec4 wpos = model_matrix * vec4(position,1);
	epos = view_matrix * wpos;
	gl_Position = projection_matrix * epos;

	// pass tc and eye-space normal to fragment shader
	norm = normalize(mat3(view_matrix*model_matrix)*normal);
	norm_origin = normalize(normal);
	matrix = mat3(view_matrix*model_matrix);

	// text
	if (is_text == 1) {
		gl_Position = text_matrix * vec4(position, 1.0);
	}
	if (is_title == 1) {
		gl_Position = aspect_matrix * vec4(position, 1);
		if (is_back == 1) {
			gl_Position = vec4(position, 1);
		}
		tc = texcoord;
	}

}