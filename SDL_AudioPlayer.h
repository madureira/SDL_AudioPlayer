#pragma once

#include<string.h>
#include <SDL2/SDL.h>

#define MAX_VOLUME SDL_MIX_MAXVOLUME
static const int MAX_SONGS = 64;
static SDL_AudioFormat audio_format = AUDIO_S16;
static const char* playing_music_name = "";
static const char* next_music_on_queue = "";
bool is_playing_music = false;

static inline void audio_callback(void* userdata, Uint8* stream, int len);

struct AudioData
{
	SDL_AudioSpec wavSpec;
	Uint32 wavLength;
	Uint8* wavBuffer;
	const char* name;
	int volume;
	int loop;
	bool isMusic;
	bool isPaused;

	AudioData()
	{
	}
};

AudioData stored_audio_data[MAX_SONGS];

class SDL_AudioPlayer
{
private:
	SDL_AudioDeviceID m_DeviceId;
	bool m_IsDeviceOpened = false;
	bool m_IsDevicePlaying = false;
	int m_AudioFrequency;
	int m_Channels;
	int m_Samples;

public:
	SDL_AudioPlayer(SDL_AudioFormat format = AUDIO_S16, int frequency = 44100, int channels = 2, int samples = 4096) :
		m_AudioFrequency(frequency), m_Channels(channels), m_Samples(samples)
	{
		if (!(SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO))
		{
			SDL_Init(SDL_INIT_AUDIO);
		}
		audio_format = format;
	}

	~SDL_AudioPlayer()
	{
		SDL_PauseAudioDevice(m_DeviceId, 1);
		SDL_CloseAudioDevice(m_DeviceId);

		for (int i = 0; i < MAX_SONGS; i++)
		{
			if (stored_audio_data[i].wavBuffer != nullptr)
			{
				stored_audio_data[i].wavSpec.callback = NULL;
				SDL_FreeWAV(stored_audio_data[i].wavBuffer);
			}
		}
	}

	void loadMusic(const char* filePath, const char* audioName)
	{
		createAudio(filePath, audioName, true);
	}

	void loadSoundEffect(const char* filePath, const char* audioName)
	{
		createAudio(filePath, audioName, false);
	}

	void play(const char* audioName, int volume, int loop)
	{
		int index = findAudioIndexByName(audioName);
		if (index == -1)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fail to play: %s, audio not found!\n", audioName);
			return;
		}

		stored_audio_data[index].loop = loop;
		stored_audio_data[index].volume = fixVolume(volume);
		if (stored_audio_data[index].isMusic)
		{
			if (is_playing_music)
			{
				next_music_on_queue = audioName;
				stored_audio_data[index].isPaused = true;
			}
			else
			{
				is_playing_music = true;
				playing_music_name = audioName;
				stored_audio_data[index].isPaused = false;
			}
		}
		else
		{
			stored_audio_data[index].isPaused = false;
		}

		if (!m_IsDevicePlaying)
		{
			SDL_PauseAudioDevice(m_DeviceId, 0);
			m_IsDevicePlaying = true;
		}
	}

	void pause(const char* audioName)
	{
		int index = findAudioIndexByName(audioName);
		if (index == -1)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fail to pause: %s, audio not found!\n", audioName);
			return;
		}
		stored_audio_data[index].isPaused = true;
	}

	void resume(const char* audioName, int volume)
	{
		int index = findAudioIndexByName(audioName);
		if (index == -1)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fail to resume: %s, audio not found!\n", audioName);
			return;
		}
		stored_audio_data[index].volume = fixVolume(volume);
		stored_audio_data[index].isPaused = false;
	}

	void createAudio(const char* filePath, const char* audioName, bool isMusic)
	{
		if ((strcmp(filePath, "") == 0))
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Cannot create a new audio without a file path! %s\n",
				audioName);
			return;
		}

		if ((strcmp(audioName, "") == 0))
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Cannot create a new audio without name! %s\n", filePath);
			return;
		}

		int emptySlot = -1;
		for (int i = 0; i < MAX_SONGS; i++)
		{
			if (stored_audio_data[i].name == nullptr)
			{
				emptySlot = i;
				break;
			}
		}

		if (emptySlot == -1)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Exceeded the number of simultaneous audios.\nMAX_SONGS = %i\n",
				MAX_SONGS);
			return;
		}

		SDL_LockAudioDevice(m_DeviceId);

		SDL_memset(&stored_audio_data[emptySlot], 0, sizeof(AudioData));

		if (SDL_LoadWAV(filePath, &stored_audio_data[emptySlot].wavSpec, &stored_audio_data[emptySlot].wavBuffer,
			&stored_audio_data[emptySlot].wavLength) == NULL)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not open %s: %s\n", filePath, SDL_GetError());
		}
		else
		{
			stored_audio_data[emptySlot].isPaused = true;
			stored_audio_data[emptySlot].isMusic = isMusic;
			stored_audio_data[emptySlot].wavSpec.freq = m_AudioFrequency;
			stored_audio_data[emptySlot].wavSpec.format = audio_format;
			stored_audio_data[emptySlot].wavSpec.channels = m_Channels;
			stored_audio_data[emptySlot].wavSpec.samples = m_Samples;

			if (!m_IsDeviceOpened)
			{
				stored_audio_data[emptySlot].wavSpec.userdata = &stored_audio_data[emptySlot];
				stored_audio_data[emptySlot].wavSpec.callback = audio_callback;
				m_DeviceId = SDL_OpenAudioDevice(NULL, 0, &stored_audio_data[emptySlot].wavSpec, NULL, 0);

				if (m_DeviceId == 0)
				{
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not open AudioDevice: %s\n", SDL_GetError());
				}
				else
				{
					m_IsDeviceOpened = true;
				}
			}

			stored_audio_data[emptySlot].name = audioName;
		}
		SDL_UnlockAudioDevice(m_DeviceId);
	}

private:
	int findAudioIndexByName(const char* audioName)
	{
		for (int i = 0; i < MAX_SONGS; i++)
		{
			if (stored_audio_data[i].wavBuffer != nullptr && stored_audio_data[i].name == audioName)
			{
				return i;
			}
		}
		return -1;
	}

	int fixVolume(int volume)
	{
		if (volume > MAX_VOLUME)
		{
			volume = MAX_VOLUME;
		}
		else if (volume < 0)
		{
			volume = 0;
		}
		return volume;
	}
};

static inline void audio_callback(void* userdata, Uint8* stream, int len)
{
	AudioData* audioPtr = (AudioData*)userdata;

	SDL_memset(stream, 0, len);

	if (audioPtr->wavLength == 0)
	{
		return;
	}

	for (int i = 0; i < MAX_SONGS; i++)
	{
		if (stored_audio_data[i].wavLength <= 0)
		{
			continue;
		}

		if (is_playing_music && stored_audio_data[i].isMusic && stored_audio_data[i].volume > 0
			&& stored_audio_data[i].name == playing_music_name && !(strcmp(next_music_on_queue, "") == 0))
		{
			stored_audio_data[i].volume--;

			if (stored_audio_data[i].volume == 0)
			{
				is_playing_music = false;
			}
		}

		if (!is_playing_music && stored_audio_data[i].isMusic && stored_audio_data[i].volume > 0
			&& stored_audio_data[i].name == next_music_on_queue)
		{
			next_music_on_queue = "";
			is_playing_music = true;
			playing_music_name = stored_audio_data[i].name;
			stored_audio_data[i].isPaused = false;
		}

		if (stored_audio_data[i].volume > 0 && !stored_audio_data[i].isPaused)
		{
			Uint32 tempLen = (Uint32)len;

			len = (tempLen > stored_audio_data[i].wavLength ? stored_audio_data[i].wavLength : tempLen);

			SDL_MixAudioFormat(stream, stored_audio_data[i].wavBuffer, audio_format, tempLen,
				stored_audio_data[i].volume);

			stored_audio_data[i].wavBuffer += tempLen;
			stored_audio_data[i].wavLength -= tempLen;
		}
	}
}
