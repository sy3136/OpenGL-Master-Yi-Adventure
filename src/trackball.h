#ifndef __TRACKBALL_H__
#define __TRACKBALL_H__
#include "cgmath.h"

struct trackball
{
	bool	b_tracking = false;
	float	scale;			// controls how much rotation is applied
	mat4	view_matrix0;	// initial view matrix
	mat4	view_matrix0_forRotate;
	vec3	saved_eye;
	vec3	saved_at;
	vec3	return_at;
	vec3	saved_up;
	vec2	m0;				// the last mouse position

	trackball( float rot_scale=1.0f ) : scale(rot_scale){}
	bool is_tracking() const { return b_tracking; }
	void begin( const mat4& view_matrix, vec2 m, vec3 at );
	vec3 end() { b_tracking = false; return return_at; }
	mat4 updateRotate( vec2 m ) const;
	mat4 updateZoom(vec2 m) const;
	mat4 updatePan(vec2 m);
	vec3 updateAt(vec2 m);
};

vec3 get_eye_from_view(mat4 view_matrix) {
	mat4 a = view_matrix.inverse();

	return vec3(a._14, a._24, a._34);
}

vec3 get_up_from_view(mat4 view_matrix) {
	return vec3(view_matrix._21, view_matrix._22, view_matrix._23);
}

inline void trackball::begin( const mat4& view_matrix, vec2 m, vec3 at)
{
	b_tracking = true;			// enable trackball tracking
	m0 = m;						// save current mouse position
	view_matrix0 = view_matrix;	// save current view matrix
	saved_eye = get_eye_from_view(view_matrix);
	saved_at = at;
	return_at = at;
	saved_up = get_up_from_view(view_matrix);

	view_matrix0_forRotate = mat4::look_at(saved_eye - at, vec3(0, 0, 0), saved_up);
}

inline mat4 trackball::updateRotate( vec2 m ) const
{
	// project a 2D mouse position to a unit sphere
	static const vec3 p0 = vec3(0,0,1.0f);	// reference position on sphere
	vec3 p1 = vec3(m-m0,0);					// displacement
	if( !b_tracking || length(p1)<0.0001f ) return view_matrix0;		// ignore subtle movement
	p1 *= scale;														// apply rotation scale
	p1 = vec3(p1.x,p1.y,sqrtf(std::max(0.0f,1.0f-length2(p1)))).normalize();	// back-project z=0 onto the unit sphere

	// find rotation axis and angle in world space
	// - trackball self-rotation should be done at first in the world space
	// - mat3(view_matrix0): rotation-only view matrix
	// - mat3(view_matrix0).transpose(): inverse view-to-world matrix
	vec3 v = mat3(view_matrix0_forRotate).transpose()*p0.cross(p1);
	float theta = asin(std::min(v.length(),1.0f) );

	// resulting view matrix, which first applies
	// trackball rotation in the world space
	mat4 ret = view_matrix0_forRotate * mat4::rotate(v.normalize(), theta);
	vec3 eye = get_eye_from_view(ret);
	vec3 up = get_up_from_view(ret);
	return mat4::look_at(eye + saved_at, saved_at, up);
}

inline mat4 trackball::updateZoom(vec2 m) const
{
	vec3 dp = vec3(m - m0, 0);					// displacement
	vec3 dir = (saved_at - saved_eye).normalize();
	vec3 dist = saved_at - saved_eye;
	vec3 eye = saved_eye + dir * dp.y * 10.0f;
	if (dp.y > 0 && (dir * dp.y * 10.0f).length() >= dist.length()) eye = saved_at-dir*0.1f;
	mat4 ret = mat4::look_at(eye, saved_at, saved_up);
	return ret;
}

inline mat4 trackball::updatePan(vec2 m)
{
	vec3 y_axis = (view_matrix0._21, view_matrix0._22, view_matrix0._23);
	vec3 x_axis = (view_matrix0._11, view_matrix0._12, view_matrix0._13);
	vec3 dp = vec3(m - m0, 0);
	vec3 right = vec3(view_matrix0._11, view_matrix0._12, view_matrix0._13);
	vec3 move = -right * dp.x * 10.0f - saved_up * dp.y * 10.0f;
	return_at = saved_at + move;
	return mat4::look_at(saved_eye + move, saved_at + move, saved_up);
}

inline vec3 trackball::updateAt(vec2 m)
{
	vec3 y_axis = (view_matrix0._21, view_matrix0._22, view_matrix0._23);
	vec3 x_axis = (view_matrix0._11, view_matrix0._12, view_matrix0._13);
	vec3 dp = vec3(m - m0, 0);
	vec3 right = vec3(view_matrix0._11, view_matrix0._12, view_matrix0._13);
	vec3 move = -right * dp.x * 10.0f - saved_up * dp.y * 10.0f;
	return_at = saved_at + move;
	return return_at;
}

// utility function
inline vec2 cursor_to_ndc( dvec2 cursor, ivec2 window_size )
{
	// normalize window pos to [0,1]^2
	vec2 npos = vec2( float(cursor.x)/float(window_size.x-1),
					  float(cursor.y)/float(window_size.y-1) );
	
	// normalize window pos to [-1,1]^2 with vertical flipping
	// vertical flipping: window coordinate system defines y from
	// top to bottom, while the trackball from bottom to top
	return vec2(npos.x*2.0f-1.0f,1.0f-npos.y*2.0f);
}

#endif // __TRACKBALL_H__
