# SDL_AudioPlayer
A C++/SDL2 audio player compatible with Emscripten.

This library doesn't use [**SDL_mixer**](https://www.libsdl.org/projects/SDL_mixer).

[![language](https://img.shields.io/badge/language-c++-blue.svg)](https://isocpp.org)
[![c++](https://img.shields.io/badge/std-c++11-blue.svg)](https://isocpp.org/wiki/faq/cpp11)
[![SDL](https://img.shields.io/badge/SDL-2.0.9-green.svg)](https://www.libsdl.org/download-2.0.php)
[![emsdk-1.38.24](https://img.shields.io/badge/emsdk-1.38.24-red.svg)](http://kripken.github.io/emscripten-site/docs/getting_started/downloads.html)
![stability-wip](https://img.shields.io/badge/stability-work_in_progress-lightgrey.svg)

## Installation

Just include the header `SDL_AudioPlayer.h` in your project.


## How to use

### Playing a music

```cpp
#include "SDL_AudioPlayer.h"

SDL_AudioPlayer audioPlayer;

audioPlayer.loadMusic("path_to_file/music.wav", "my_music");

audioPlayer.play("my_music", MAX_VOLUME, 0);

/* Do something that takes a while */

audioPlayer.freeAudio();
```

### Playing a sound effect

```cpp
audioPlayer.loadSoundEffect("path_to_file/effect.wav", "my_cool_sound_effect");

audioPlayer.play("my_cool_sound_effect", 30, 0);
```

### Pausing and resuming a music/sound effect

```cpp
audioPlayer.play("my_song", MAX_VOLUME, 0);

/* Do something that takes a while */

audioPlayer.pause("my_song");

/* Do something that takes a while */

audioPlayer.resume("my_song", MAX_VOLUME, 0);
```


### Changing a music

If there is a music playing, and you load and play another music, the current music will fade out and the last one will be played.

```cpp
audioPlayer.loadMusic("path_to_file/music.wav", "nice_music");
audioPlayer.loadMusic("path_to_file/another_music.wav", "other_song");

audioPlayer.play("nice_music", 100, 0);

/* Do something that takes a while */

audioPlayer.play("other_song", MAX_VOLUME, 0);
```

In the example above, the `nice_music` will start to play. Immediately, it will start to fade out, because the `other_song` signalized that it needs to play.


### Clean up the memory

After using the SDL_AudioPlayer instance, it's a good idea to finish it.

You cand do this calling the method `quit()`.

```cpp
audioPlayer.loadMusic("path_to_file/music.wav", "nice_music");

audioPlayer.play("nice_music", 100, 0);

/* Do something that takes a while */

// frees the audio memory and finishes SDL_AudioPlayer
audioPlayer.quit();
```

### Changing audio volume

It's possible to change the audio volume at runtime.

```cpp
audioPlayer.loadMusic("path_to_file/music.wav", "nice_music");

audioPlayer.play("nice_music", 100, 0);

/* Do something that takes a while */

audioPlayer.volume("nice_music", 20);
```

## Limitations

- Only **.WAV** files.
- Only **64** audios simultaneously.
