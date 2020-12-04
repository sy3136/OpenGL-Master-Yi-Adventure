#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

// input from vertex shader
in vec4 epos;
in vec3 norm;
in vec2 tc;
in vec3 norm_origin;
in mat3 matrix;

// the only output variable
out vec4 fragColor;

uniform mat4	view_matrix;
uniform float	shininess;
uniform vec4	light_position, Ia, Id, Is;	// light
uniform vec4	Ka, Kd, Ks;					// material properties

// texture sampler
uniform sampler2D TEX;
uniform sampler2D ALPHATEX;
uniform sampler2D BUMPTEX;
// Text
uniform sampler2D	TEXT;
uniform vec4		text_color;
uniform int is_text;
uniform int is_title;

uniform int mode;
uniform int alphablending;
uniform int bump;

vec4 phong( vec3 l, vec3 n, vec3 h, vec4 Kd )
{
	vec4 Ira = Ka*Ia;									// ambient reflection
	vec4 Ird = max(Kd*dot(l,n)*Id,Kd * 0.3);					// diffuse reflection
	vec4 Irs = max(Ks*pow(dot(h,n),shininess)*Is,0.0);	// specular reflection
	return Ira + Ird + Irs;
}

vec3 computeNormal_backup( vec2 tc, 
                    vec3 tangent, 
                    vec3 binormal)
{
	mat3 J;
	
	vec4 cur = texture(BUMPTEX, vec2(tc.x, tc.y));
	vec4 leftx = texture( BUMPTEX, vec2(tc.x+0.001f, tc.y) );
	vec4 downz = texture(BUMPTEX, vec2(tc.x, tc.y+0.002f));
	J[0][0] = 1.0;
	J[0][1] = (cur.r - leftx.r)*7.0f;
	J[0][2] = 0.0;
	
	J[1][0] = 0.0;
	J[1][1] = 1.0;
	J[1][2] = 0.0;

	J[2][0] = 0.0;
	J[2][1] = (cur.r - downz.r)*7.0f;
	J[2][2] = 1.0;
	
	vec3 u = J * tangent;
	vec3 v = J * binormal;
	
	vec3 n = cross(v, u);
	return normalize(n);
}

void main()
{
	vec4 lpos = view_matrix*light_position;
	vec3 normal = norm;
	if(bump == 1) {
		vec4 height = texture( BUMPTEX, tc );
		vec3 tangent; 
		vec3 binormal;

		float c = cos(1.57f);
		float s = sin(1.57f);
		tangent = vec3(c*norm_origin.x-s*norm_origin.y, s*norm_origin.x+c*norm_origin.y, 0);
		tangent = normalize(tangent);
	
		binormal = cross(norm_origin, tangent); 
		binormal = normalize(binormal);

		normal = computeNormal_backup(tc, -tangent, binormal);
		normal = normalize(matrix*normal);
	}

	vec3 n = normalize(normal);	// norm interpolated via rasterizer should be normalized again here
	vec3 p = epos.xyz;			// 3D position of this fragment
	vec3 l = normalize(lpos.xyz-(lpos.a==0.0?vec3(0):p));	// lpos.a==0 means directional light
	vec3 v = normalize(-p);		// eye-epos = vec3(0)-epos
	vec3 h = normalize(l+v);	// the halfway vector

	vec4 iKd = texture( TEX, tc );	// Kd from image
	if(mode==0)			fragColor = phong( l, n, h, iKd );
	else if(mode==1)	fragColor = phong( l, n, h, Kd );
	else if(mode==2)	fragColor = iKd;
	else				fragColor = vec4( tc, 0, 1 );

	if(alphablending == 1)
	{
		vec4 at = texture( ALPHATEX, tc);
		if(at.r <= 0.2 && at.g <= 0.2 && at.b <= 0.2) discard;
	}

	if (is_text == 1) {
		float alpha = texture(TEXT, tc).r;
		fragColor = text_color * vec4(1, 1, 1, alpha);
	}

	if (is_title == 1)
	{
		fragColor = texture(TEX, tc);
	}
}
