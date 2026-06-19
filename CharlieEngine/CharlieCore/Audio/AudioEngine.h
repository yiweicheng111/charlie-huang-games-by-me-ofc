#include "miniaudio/miniaudio.h"
#include <iostream>
#include <glm/glm.hpp>
#include <string>
namespace Cle::Audio {
	
	class Sound
	{
	private:
		bool playing = false;
	public:
		bool isPlaying()
		{
			return playing;
		}
		ma_sound sound;
		float volume = 1;
		bool global = false;
		std::string path;
		glm::vec3 position = glm::vec3(0.0f);
		~Sound()
		{
			ma_sound_uninit(&sound);
		}
		explicit Sound(std::string path, ma_engine& engine) : path(path) {
			ma_sound_init_from_file(&engine, path.c_str(), MA_SOUND_FLAG_ASYNC | MA_SOUND_FLAG_DECODE, nullptr, nullptr, &sound);
		}
		void Play()
		{
			playing = true;
			ma_sound_seek_to_pcm_frame(&sound, 0);
			ma_sound_start(&sound);
		}
		void UpdateVolume(glm::vec3 observor)
		{
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