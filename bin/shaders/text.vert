layout (location=0) in vec3 position;
layout (location=2) in vec2 texcoord;

out vec2 tc;

uniform mat4 text_matrix;
uniform mat4 aspect_matrix;

uniform int back;

void main()
{
    gl_Position = aspect_matrix *  text_matrix *  vec4(position,1.0);
    if (back == 1) {
        gl_Position = text_matrix * vec4(position, 1.0);
    }
    tc = texcoord;
}  
