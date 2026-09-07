#pragma once
#include "miniaudio/miniaudio.h"
#include <iostream>
#include <glm/glm.hpp>
#include <string>
namespace Cle::Audio {
	
	class Sound
	{
	private:
		bool playing = false;
		std::string path;
		
	public:
		float volume = 1;
		bool isPlaying() const
		{
			return playing;
		}
		void setPlaying(bool v)
		{
			playing = v;
		}
		ma_sound sound;
		bool global = false;
		ma_engine* engine;
		glm::vec3 position = glm::vec3(0.0f);
		~Sound()
		{
			ma_sound_uninit(&sound);
		}
		explicit Sound(std::string path, ma_engine* engine) : path(path), engine(engine) {
			ma_sound_init_from_file(engine, path.c_str(), MA_SOUND_FLAG_ASYNC | MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_STREAM, nullptr, nullptr, &sound);
			ma_sound_seek_to_pcm_frame(&sound, 0);
			ma_sound_set_end_callback(&sound, [](void* pUserData, ma_sound* pSound) {
				auto pl = static_cast<bool*>(pUserData);
				*pl = false;
				}, &playing);

		}
		explicit Sound(std::string path, ma_engine* engine,bool globa) : path(path), engine(engine), global(globa) {
			ma_sound_init_from_file(engine, path.c_str(), MA_SOUND_FLAG_ASYNC | MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_STREAM, nullptr, nullptr, &sound);
			ma_sound_seek_to_pcm_frame(&sound, 0);
			ma_sound_set_end_callback(&sound, [](void* pUserData, ma_sound* pSound) {
				auto pl = static_cast<bool*>(pUserData);
				*pl = false;
				}, &playing);

		}
		 void setPath(std::string newPath)
		{
			ma_sound_uninit(&sound);
			ma_sound_init_from_file(engine, path.c_str(), MA_SOUND_FLAG_ASYNC | MA_SOUND_FLAG_DECODE, nullptr, nullptr, &sound);
			path = newPath;
		}
		 std::string getPath() const
		{
			return path;
		}
		float getTimePosition()
		{
			unsigned long long frame;
			ma_sound_get_cursor_in_pcm_frames(&sound, &frame);
			return (double)frame / ma_engine_get_sample_rate(engine);
		}
		void setTimePosition(float seconds)
		{
			ma_sound_seek_to_pcm_frame(&sound, ma_engine_get_sample_rate(engine) * seconds);
		}
		void Play()
		{	playing = true;
			ma_sound_start(&sound);
		}
		void Pause()
		{
			playing = false;
			ma_sound_stop(&sound);
		}
		void UpdateVolume(glm::vec3 observor)
		{
			if (!playing) return;
			if (global) {
				ma_sound_set_volume(&sound, volume);
				ma_sound_start(&sound);
				return;
			}
			float drop = glm::distance(observor, position);
			if (drop < 1) drop = 1;

			float vol = volume / (drop);

			ma_sound_set_volume(&sound, vol);

		}
	};
}