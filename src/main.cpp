#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility
#include "trackball.h"	// virtual trackball
#include "utils.h"
#include "Box.h"
#include "title.h"
#include "help.h"
#include "Weapon.h"
#include "Character.h"
#include "Zombie.h"
#include "Grass.h"
#include "Tree.h"

// model
#include "assimp_loader.h"
#include "cgut2.h"
#include "tree.h"

#include "irrKlang\irrKlang.h" // Sound
#pragma comment(lib, "irrKlang.lib")



#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//*******************************************************************
// forward declarations for freetype text
bool init_text();
void render_text(std::string text, GLint x, GLint y, GLfloat scale, vec4 color, GLfloat dpi_scale = 1.0f);
void render_text_3d(std::string text, GLint x, GLint y, GLfloat scale, vec4 color, GLfloat dpi_scale = 1.0f, GLuint program = 0, int is_text = 1);


//*************************************
// global constants
static const char*	window_name = "cgbase - trackball";
static const char*	vert_shader_path = "../bin/shaders/trackball.vert";
static const char*	frag_shader_path = "../bin/shaders/trackball.frag";
//static const char* wave_path = "../bin/sounds/strange-alarm.wav"; // Sound
static const char* wave_path = "../bin/sounds/ophelia.mp3"; // Sound
static const char* hit_sound_path = "../bin/sounds/punch.wav"; // Sound
static const char* pain_sound_path = "../bin/sounds/Pain.wav"; // Sound

static const char*	texture_paths[19] = {
	"../bin/textures/ground.bmp", "../bin/textures/skybox/Daylight Box UV.jpg", "../bin/textures/skybox/1.jpg"
};
//*************************************
// common structures
vec3 campos = vec3(1.75f, 15, 15);
struct camera
{
	vec3	eye = vec3(campos.x, campos.y, campos.z);
	vec3	at = vec3( 0, 0, 0 );
	vec3	up = vec3( -campos.x, -campos.y, 0 );
	mat4	view_matrix = mat4::look_at( eye, at, up );

	float	fovy = PI/4.0f; // must be in radian
	float	aspect;
	float	dnear = 1.0f;
	float	dfar = 1000.0f;
	mat4	projection_matrix;
};

struct light_t
{
	vec4	position = vec4(500.0f, 500.0f, 1000.0f, 1.0f);   // directional light
	vec4	ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	vec4	diffuse = vec4(0.8f, 0.8f, 0.8f, 1.0f);
	vec4	specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
};

struct material_t
{
	vec4	ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	vec4	diffuse = vec4(0.8f, 0.8f, 0.8f, 1.0f);
	vec4	specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	float	shininess = 100000000.0f;
};

//*************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = cg_default_window_size(); // initial window size

//*************************************
// OpenGL objects
GLuint	program	= 0;	// ID holder for GPU program

//*************************************
// global variables
int		frame = 0;				// index of rendering frames
int		mode = 0;

//*************************************
// scene objects
camera		cam;
trackball	tb;
light_t		light;
material_t	material;
//*************************************
// Sound
// irrKlang objects
irrklang::ISoundEngine* engine = nullptr;
irrklang::ISound* sound = nullptr;
irrklang::vec3df sound_pos(0, 0, 0);				// position of the sound source
irrklang::vec3df sound_velocity(0, 0, 0);			// velocity of the sound source - for Doppler effect

//*************************************
// Text

float	a = 0.0f;
int		scene = 0;
//*************************************

// Title
GLuint	SRC = 0;	// texture objects
GLuint  vertex_array_title = 0;	// ID holder for vertex array object

// Title
GLuint	SRC_help = 0;	// texture objects
GLuint	back_help = 0;	// texture objects
GLuint  vertex_array_help = 0;	// ID holder for vertex array object
GLuint  vertex_array_back_help = 0;	// ID holder for vertex array object

static const char* image_path = "../bin/images/title4.jpg";
static const char* image_path_help = "../bin/images/title2.jpg";
static const char* image_path_help_back = "../bin/images/back.jpg";

//*************************************
// model
std::vector<Tree> trees;
bool is_model = false;
int num_mesh = 10;
static const char* mesh_obj = "../bin/mesh/Tree/CartoonTree.3ds";
static const char* mesh_3ds = "../bin/mesh/head/head.3ds";
//*************************************

bool pause = false;
bool reset = false;
bool wireframe = false;
float t = 0.0f;
float check_frame = 0.0f;
float delta_frame = 0.0f;

GLuint  box_vertex_array = 0;
GLuint  skybox_vertex_array = 0;
GLuint	grass_vertex_array = 0;
int		box_poligon_num = 12;

bool updateRotating = false;
bool updatePanning = false;
bool updateZooming = false;

bool shift_key = false;
bool control_key = false;
bool key_A = false, key_D = false, key_W = false, key_S = false;
bool key_attack = false;
bool hit_attack = false;
int	 hit_id = 0;
float hit_time = float(glfwGetTime());
float hit_vec = 0;
float hit_vec2 = 0;

bool is_zombie_dead = false;

float knock_back_scale = 2.0f;

Box ground;
Box skybox;
std::vector<Grass> grass;
Character character;
bool character_attack = false;
//Zombie zombie;
std::vector<Zombie> zombie;
int num_zombie = 3;

// Sound
void update_sound() {
	if (sound)
	{
		sound->setMinDistance(2.0f);	//minimum distance between a listener and the 3D sound source
	}
	//set Dopper effect parameters
	// two times than the real world Doppler effect
	engine->setDopplerEffectParameters(2.0f, 1.0f);

	//set listener's attributes using camera attributes
	irrklang::vec3df listener_pos = reinterpret_cast<irrklang::vec3df&> (cam.eye);			// listener's position
	irrklang::vec3df listener_up = reinterpret_cast<irrklang::vec3df&> (cam.up);		// listener's up vector
	irrklang::vec3df look_direction = reinterpret_cast<irrklang::vec3df&> (cam.at);		// the direction the listener looks into
	irrklang::vec3df listener_velocity(0, 0, 0);

	//change OpenGL coordinates to irrKlang coordinates
	listener_pos.Z = -listener_pos.Z;
	look_direction.Z = -look_direction.Z;
	listener_up.Z = -listener_up.Z;

	engine->setListenerPosition(listener_pos, look_direction, listener_velocity, listener_up);
}

void update()
{
	
	if (!pause) t += delta_frame;
	// update projection matrix
	cam.aspect = window_size.x/float(window_size.y);
	cam.projection_matrix = mat4::perspective( cam.fovy, cam.aspect, cam.dnear, cam.dfar );
	// Sound
	/*
	sound_pos = irrklang::vec3df(cos(t) * 10.0f, 0, -30.0f + sin(t) * 20.0f);			//circular motion
	sound_velocity = sound_pos - irrklang::vec3df(cos(t0) * 10.0f, 0, -20.0f + sin(t0) * 10.0f);	//calculate the instantaneous velocity of sound source
	if (sound)
	{
		sound->setPosition(sound_pos);
		sound->setVelocity(sound_velocity);
	}*/

	//Text
	a = abs(sin(float(glfwGetTime()) * 2.5f));
	ground.update(t, vec3(0, 0, 0), 0);
	for (auto& g : grass) g.update(t);
	skybox.update(t, vec3(0, 0, 0), 0);

	character.update(t, delta_frame);
	int cnt = 0;
	for (auto& z : zombie) {
		if (z.life != 0) {
			z.update(t, delta_frame, character.pos);
		}
		else {
			zombie.erase(zombie.begin() + cnt);
			num_zombie--;
			break;
		}
		cnt++;
	}

	if (key_attack) {
		character.attack(t);
		character_attack = true;
		key_attack = false;
	}
	for (auto& z : zombie) {
		if (z.hit) {
			character.life--;
			if (character.life <= 0) {
				character.life = 0;
			}
			sound_pos = irrklang::vec3df(0, 0, 0);
			sound = engine->play3D(pain_sound_path, sound_pos, false, false, false);
			update_sound();
		}
	}
	if (character_attack) {
		if (!character.isAttacking(t)) {
			for (auto& z : zombie) {
				character_attack = false;
				vec3 pos = character.getAttackingPos();
				vec3 pos2 = z.getPos();
				if ((z.getPos() - pos).length() < 2.0f) {
					hit_attack = true;
					hit_time = float(glfwGetTime());
					hit_vec = float(0.06 * cos(atan2((cam.at - z.getPos()).y, (cam.at - z.getPos()).x)));
					hit_vec2 = float(0.05 * sin(atan2((cam.at - z.getPos()).y, (cam.at - z.getPos()).x)));
					//engine->removeAllSoundSources();
					//engine->removeSoundSource(sound->getSoundSource());
					sound_pos = irrklang::vec3df(float(z.getPos().x), float(z.getPos().y), float(z.getPos().z));
					sound = engine->play3D(hit_sound_path, sound_pos, false, false, false);
					update_sound();
					z.knockback((z.getPos() - character.getPos()).normalize(), knock_back_scale);
					//sound_pos = irrklang::vec3df(0, 0, 0);
					//hit_id = z.id;
				}
			}
		}
	}

	
	float hit_t = float(glfwGetTime());
	if (hit_t - hit_time >= 0.5f)
		hit_attack = false;

	cam.at = character.getPos();
	cam.eye = vec3(campos.x, campos.y, campos.z) + character.getPos();
	cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);


	// update uniform variables in vertex/fragment shaders
	GLint uloc;
	uloc = glGetUniformLocation(program, "view_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.view_matrix);
	uloc = glGetUniformLocation(program, "projection_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.projection_matrix);
	glUniform1i(glGetUniformLocation(program, "is_model"), 1);
	for (auto& t : trees) t.update(program);
	glUniform1i(glGetUniformLocation(program, "is_model"), 0);
	// setup light properties
	glUniform4fv(glGetUniformLocation(program, "light_position"), 1, light.position);
	glUniform4fv(glGetUniformLocation(program, "Ia"), 1, light.ambient);
	glUniform4fv(glGetUniformLocation(program, "Id"), 1, light.diffuse);
	glUniform4fv(glGetUniformLocation(program, "Is"), 1, light.specular);
	// setup material properties
	glUniform4fv(glGetUniformLocation(program, "Ka"), 1, material.ambient);
	glUniform4fv(glGetUniformLocation(program, "Kd"), 1, material.diffuse);
	glUniform4fv(glGetUniformLocation(program, "Ks"), 1, material.specular);
	glUniform1f(glGetUniformLocation(program, "shininess"), material.shininess);
}

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	if (scene == 1) {

		// Sound
		if (sound) {
			sound->stop(); // release sound stream.
		}
		/* 사운드 멈추게 하는 것if (sound->getIsPaused()) {
			sound->setIsPaused(false);
		}*/
		glDepthFunc(GL_LESS);
		// notify GL that we use our own program
		glUseProgram(program);
		glBindVertexArray(grass_vertex_array);
		for (auto& g : grass) g.render(program, 2);

		glBindVertexArray(box_vertex_array);
		ground.render(program, 2, 0);
		character.render(program);
		for (auto& z : zombie) {
			if (z.life != 0) {
				z.render(program);
			}
		}

		// model
		for (auto& t : trees) t.render(program);

		glBindVertexArray(skybox_vertex_array);
		skybox.render(program, 2, 0);

		float dpi_scale = cg_get_dpi_scale();
		for (auto& z : zombie) {
			if (hit_attack && z.knockbacking) {
				// if(hit_id == z.id) // One-by-One hit if want.
					render_text_3d("Hit!", int(window_size.x * (0.47f + hit_vec + 0.02f * (character.getPos() - z.getPos()).x)), int(window_size.y * (0.3500f - hit_vec2 - 0.01f * (character.getPos() - z.getPos()).y)), 1.0f, vec4(255.0f, 0.0f, 0.0f, 1.0f), dpi_scale, program, 1);
			}
		}

		if (character.life == 3)
			render_text_3d("Life: X X X", int(window_size.x * 0.75), int(window_size.y*0.9), 0.85f, vec4(255.0f, 0.0f, 0.0f, 1.0f), dpi_scale, program, 1);
		else if (character.life == 2)
			render_text_3d("Life: X X", int(window_size.x * 0.75), int(window_size.y*0.9), 0.85f, vec4(255.0f, 0.0f, 0.0f, 1.0f), dpi_scale, program, 1);
		else if (character.life == 1)
			render_text_3d("Life: X", int(window_size.x * 0.75), int(window_size.y*0.9), 0.85f, vec4(255.0f, 0.0f, 0.0f, 1.0f), dpi_scale, program, 1);
		else
			render_text_3d("Life:", int(window_size.x * 0.75), int(window_size.y * 0.9), 0.85f, vec4(255.0f, 0.0f, 0.0f, 1.0f), dpi_scale, program, 1);

		if (pause) {
			//sound->setIsPaused(true);
			glDepthFunc(GL_ALWAYS);
			dpi_scale = cg_get_dpi_scale();
			render_help(program, SRC_help, back_help, vertex_array_help, vertex_array_back_help, window_size);
			render_text("Press F1 to resume", int(window_size.x * 0.8), int(window_size.y*0.8), 1.0f, vec4(0.0f, 0.0f, 0.0f, a), dpi_scale);
			render_text("<Help>", -int(window_size.x * 0.2), int(window_size.y * 0.07), 1.3f, vec4(255.0f, 255.0f, 255.0f, 1.0f), dpi_scale);
		}

		
	}

	else {

		glDepthFunc(GL_ALWAYS);
		// Title
		render_title(program, SRC, vertex_array_title, window_size);
		float dpi_scale = cg_get_dpi_scale();
		render_text("Click to Start", int(window_size.x * 0.7) , int(window_size.y * 0.9), 1.6f, vec4(0.0f, 0.0f, 0.0f, a), dpi_scale);
	}

	// swap front and back buffers, and display to screen
	glfwSwapBuffers( window );
}

void reshape( GLFWwindow* window, int width, int height )
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2(width,height);
	glViewport( 0, 0, width, height );
}

void print_help()
{
	printf( "[help]\n" );
	printf( "- press ESC or 'q' to terminate the program\n" );
	printf( "- press F1 to see help in game\n" );
	printf( "- press Home to reset camera\n" );
	printf( "- press R to reset \n");

	printf( "\n" );
}

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if (pause) {
		if (action == GLFW_PRESS) {
			if (key == GLFW_KEY_F1) pause = !pause;
		}
		return;
	}
	if(action==GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H)					print_help();
		else if (key == GLFW_KEY_HOME)				cam = camera();
		else if (key == GLFW_KEY_F1)				pause = !pause;
		else if (key == GLFW_KEY_R)					scene = 0;
		else if (key == GLFW_KEY_PAGE_UP) {
			wireframe = !wireframe;
			glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
		}
		else if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) shift_key = true;
		else if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) control_key = true;
		else if (key == GLFW_KEY_W) { key_W = true; character.move(t, 0, -1); }
		else if (key == GLFW_KEY_D)	{ key_D = true; character.move(t, -1, 0); }
		else if (key == GLFW_KEY_A)	{ key_A = true; character.move(t, 1, 0); }
		else if (key == GLFW_KEY_S)	{ key_S = true; character.move(t, 0, 1); }
	}
	else if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) shift_key = false;
		else if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) control_key = false;
		else if (key == GLFW_KEY_W) { key_W = false; character.stop(0, -1); }
		else if (key == GLFW_KEY_D) { key_D = false; character.stop(-1, 0); }
		else if (key == GLFW_KEY_A) { key_A = false; character.stop(1, 0); }
		else if (key == GLFW_KEY_S) { key_S = false; character.stop(0, 1); }

		if ((key == GLFW_KEY_W || key == GLFW_KEY_D || key == GLFW_KEY_A || key == GLFW_KEY_S) &&
			(!key_W && !key_D && !key_A && !key_S)) character.end_moving();
	}
}

void mouse( GLFWwindow* window, int button, int action, int mods )
{
	if (pause) {
		return;
	}
	if(button==GLFW_MOUSE_BUTTON_LEFT)
	{
		dvec2 pos; glfwGetCursorPos(window,&pos.x,&pos.y);
		//printf("> Left mouse button pressed at (%d, %d)\n", int(pos.x), int(pos.y));
		vec2 npos = cursor_to_ndc( pos, window_size );
		if (action == GLFW_PRESS)			{
			if (scene == 1) {
				tb.begin(cam.view_matrix, npos, cam.at);
				if (!key_attack) {
					key_attack = true;
					if (!character.isAttacking(t)) knock_back_scale = 2.0f;
				}
				if (control_key)
					updatePanning = true;
				else if (shift_key)
					updateZooming = true;
				else
					updateRotating = true;
			}
		}
		else if (action == GLFW_RELEASE)	{ 
			cam.at = tb.end();
			updateRotating = updateZooming = updatePanning = false;
			if(scene == 0)
				scene = 1;
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		vec2 npos = cursor_to_ndc(pos, window_size);
		if (action == GLFW_PRESS) { 
			tb.begin(cam.view_matrix, npos, cam.at);
			updateZooming = true;
			if (!key_attack) {
				key_attack = true;
				if (!character.isAttacking(t)) knock_back_scale = 5.0f;
			}
		}
		else if (action == GLFW_RELEASE) {
			tb.end();
			updateZooming = false;
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
	{
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		vec2 npos = cursor_to_ndc(pos, window_size);
		if (action == GLFW_PRESS) { tb.begin(cam.view_matrix, npos, cam.at);	updatePanning = true; }
		else if (action == GLFW_RELEASE) {
			cam.at = tb.end();
			updatePanning = false;
		}
	}
}



void motion(GLFWwindow* window, double x, double y)
{
	if (!tb.is_tracking()) return;
	vec2 npos = cursor_to_ndc(dvec2(x, y), window_size);

	if (updateRotating) {
		cam.view_matrix = tb.updateRotate(npos);
		cam.eye = get_eye_from_view(cam.view_matrix);
		cam.up = get_up_from_view(cam.view_matrix);
	}
	else if (updateZooming) {
		cam.view_matrix = tb.updateZoom(npos);
		cam.eye = get_eye_from_view(cam.view_matrix);
		cam.up = get_up_from_view(cam.view_matrix);
	}
	else if (updatePanning) {
		cam.view_matrix = tb.updatePan(npos);
		cam.eye = get_eye_from_view(cam.view_matrix);
		cam.at = tb.updateAt(npos);
		cam.up = get_up_from_view(cam.view_matrix);
	}
}

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glLineWidth(1.0f);
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests
	glEnable(GL_TEXTURE_2D);			// enable texturing
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);		// notify GL the current texture slot is 0

	
	// time init
	check_frame = t = float(glfwGetTime());

	// Sound
	// create the engine
	engine = irrklang::createIrrKlangDevice();
	if (!engine) return false;
	// play the sound source in 3D
	sound = engine->play3D(wave_path, sound_pos, true, false, true);
	if (sound)
	{
		sound->setMinDistance(5.0f);	//minimum distance between a listener and the 3D sound source
	}
	//set Dopper effect parameters
	// two times than the real world Doppler effect
	engine->setDopplerEffectParameters(2.0f, 1.0f);

	//set listener's attributes using camera attributes
	irrklang::vec3df listener_pos = reinterpret_cast<irrklang::vec3df&> (cam.eye);			// listener's position
	irrklang::vec3df listener_up = reinterpret_cast<irrklang::vec3df&> (cam.up);		// listener's up vector
	irrklang::vec3df look_direction = reinterpret_cast<irrklang::vec3df&> (cam.at);		// the direction the listener looks into
	irrklang::vec3df listener_velocity(0, 0, 0);

	//change OpenGL coordinates to irrKlang coordinates
	listener_pos.Z = -listener_pos.Z;
	look_direction.Z = -look_direction.Z;
	listener_up.Z = -listener_up.Z;

	engine->setListenerPosition(listener_pos, look_direction, listener_velocity, listener_up);

	// Text
	// setup freetype
	if (!init_text()) return false;

	srand((unsigned int)time(NULL));
	update_box_vertex_buffer(create_box_vertices(), box_vertex_array);
	update_skybox_vertex_buffer(create_skybox_vertices(), skybox_vertex_array);
	update_grass_vertex_buffer(create_grass_vertices(), grass_vertex_array);
	
	for(int i = 0; i < 500; i++)
		grass.push_back(Grass(vec3(float((rand() % 50) - 25), float((rand() % 50) - 25), 0.0f), vec3(1.0f, 1.0f, (rand() % 10) / 15.0f + 0.5f)));
	ground = Box(texture_paths[0], 100.0f, 100.0f, 1.0f, 1.0f, vec3(0,0,0));
	character = Character(vec3(0.0f, 0.0f, 0.0f), 1.0f);
	for (int i = 0; i < num_zombie; i++) {
		zombie.push_back(Zombie(vec3(float((rand() % 10) - 10), 0.0f, 0.0f), 1.0f, false, i));
	}
	skybox = Box(texture_paths[1], 1.0f, 1.0f, 1.0f, 100.0f, vec3(-100.0f, 0.0f, 0.0f), 1.0f, PI/2);

	// Title
	if (!init_title(image_path, SRC, vertex_array_title)) return false;
	// Help
	if (!init_help(image_path_help, image_path_help_back, SRC_help, back_help, vertex_array_help, vertex_array_back_help)) return false;


	// load the mesh
	for (int i = 0; i < num_mesh; i++) {
		trees.push_back(Tree(load_model(mesh_obj), vec3(1.0f, 1.0f, 1.0f), vec3(float((rand() % 40) - 20), float((rand() % 40) - 20), 0.0f)));
	}
	return true;
}

void user_finalize()
{
	// Sound
	if (sound) sound->drop(); // release sound stream.
	engine->drop(); // delete engine

	// model
	for (int i = 0; i < num_mesh; i++)
	{
		delete_texture_cache();
		delete trees[i].pMesh;
	}
}

int main( int argc, char* argv[] )
{
	// create window and initialize OpenGL extensions
	if(!(window = cg_create_window( window_name, window_size.x, window_size.y ))){ glfwTerminate(); return 1; }
	if(!cg_init_extensions( window )){ glfwTerminate(); return 1; }	// version and extensions

	// initializations and validations
	if(!(program=cg_create_program( vert_shader_path, frag_shader_path ))){ glfwTerminate(); return 1; }	// create and compile shaders/program
	if(!user_init()){ printf( "Failed to user_init()\n" ); glfwTerminate(); return 1; }					// user initialization

	// register event callbacks
	glfwSetWindowSizeCallback( window, reshape );	// callback for window resizing events
    glfwSetKeyCallback( window, keyboard );			// callback for keyboard events
	glfwSetMouseButtonCallback( window, mouse );	// callback for mouse click inputs
	glfwSetCursorPosCallback( window, motion );		// callback for mouse movement

	// enters rendering/event loop
	for( frame=0; !glfwWindowShouldClose(window); frame++ )
	{
		while (float(glfwGetTime()) - check_frame <= 0.016f);
		delta_frame = float(glfwGetTime()) - check_frame;
		if (pause) delta_frame = 0.0f;
		check_frame = float(glfwGetTime());
		
		glfwPollEvents();	// polling and processing of events
		update();			// per-frame update
		render();			// per-frame render
	}

	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}
