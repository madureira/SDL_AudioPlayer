# SDL_AudioPlayer
A C++/SDL2 audio player compatible with Emscripten.

This library doesn't use **SDL_mixer**.

[![language](https://img.shields.io/badge/language-c++-blue.svg)](https://isocpp.org)
[![c++](https://img.shields.io/badge/std-c++11-blue.svg)](https://isocpp.org/wiki/faq/cpp11)
[![SDL](https://img.shields.io/badge/SDL-2.0.9-green.svg)](https://www.libsdl.org/download-2.0.php)
[![emsdk-1.38.24](https://img.shields.io/badge/emsdk-1.38.24-red.svg)](http://kripken.github.io/emscripten-site/docs/getting_started/downloads.html)

## Install
Just include the header `SDL_AudioPlayer.h` in your project.

## How to use

### Playing a music

```c
#include "SDL_AudioPlayer.h"

SDL_AudioPlayer audioPlayer;

audioPlayer.loadMusic("path_to_file/music.wav", "my_music");

audioPlayer.play("music_one", MAX_VOLUME, 0);
```

### Playing a sound effect
```c
audioPlayer.loadSoundEffect("path_to_file/effect.wav", "my_cool_sound_effect");

audioPlayer.play("my_cool_sound_effect", 30, 0);
```

### Changing a music
If there is a music playing, and you load and play another music, the current music will fade out and the last one will be played.

```c
audioPlayer.loadMusic("path_to_file/music.wav", "nice_music");
audioPlayer.loadMusic("path_to_file/another_music.wav", "other_song");

audioPlayer.play("nice_music", 100, 0);
audioPlayer.play("other_song", MAX_VOLUME, 0);
```

In the example above, the `nice_music` will start to play. Immediately, it will start to fade out, because the `other_song` signalized that it needs to play.
