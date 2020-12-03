#pragma once
#ifndef __SOUND__
#define __SOUND__

class Sound
{
public:
	Sound() {}
	Sound(irrklang::ISoundEngine* engine = nullptr, irrklang::ISound* sound = nullptr, irrklang::vec3df sound_pos = irrklang::vec3df(0, 0, 0), irrklang::vec3df sound_velocity = irrklang::vec3df(0, 0, 0)) {

	}
};

void update_sound() {
	// Sound
	/*
	sound_pos = irrklang::vec3df(cos(t) * 10.0f, 0, -30.0f + sin(t) * 20.0f);			//circular motion
	sound_velocity = sound_pos - irrklang::vec3df(cos(t0) * 10.0f, 0, -20.0f + sin(t0) * 10.0f);	//calculate the instantaneous velocity of sound source
	if (sound)
	{
		sound->setPosition(sound_pos);
		sound->setVelocity(sound_velocity);
	}*/
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

bool init_sound(const char* wave_path, struct camera cam) {
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
}

void final_sound() {
	// Sound
	if (sound) sound->drop(); // release sound stream.
	engine->drop(); // delete engine
}

#endif