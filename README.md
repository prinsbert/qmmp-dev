# Qmmp Git Mirror 
Mirror of the source code for the [Qmmp media player](https://sourceforge.net/projects/qmmp-dev/) converted from SVN to Git

> ⚠️ **You may be interested in:** a [fork of Qmmp with improvements to the media library](https://github.com/desertmilk/qmmp-with-media-library). It adds Artist/Album filtering to the Media Library (closer to Winamp behavior), and in skinned mode applies the loaded skin to the Media Library window with snapping so it groups with the Equalizer and Playlist windows.

<img width="548" height="430" alt="Main window of qmmp media player" src="https://github.com/user-attachments/assets/a9fe56ba-fc13-43b3-bb42-1162606ff939" />

## SVN → Git mirror

**This repository is not the source of truth for Qmmp.** 

This repository is an archive tracking the [Qmmp source code](https://sourceforge.net/p/qmmp-dev/) hosted on SourceForge, but with commit history converted from SVN to Git. All real development happens in the upstream [SVN repository](https://sourceforge.net/p/qmmp-dev/code/). The sync script attempts to follow all [branches](https://github.com/prinsbert/qmmp-dev/branches) as closely as possible, where `trunk` is renamed to [`master`](https://github.com/prinsbert/qmmp-dev/tree/master).

### Mirror Status

| Field | Value |
|-------|-------|
| Latest SVN revision | [`r13291`](https://github.com/prinsbert/qmmp-dev/commit/7b59275e86665941f36d5a667bd73b59c37ba212) |
| Last synced | 2026-09-26 05:09:05 UTC |
| Branches (53) | [`feature/media-library-management`](https://github.com/prinsbert/qmmp-dev/tree/feature/media-library-management) [`master`](https://github.com/prinsbert/qmmp-dev/tree/master) [`qmmp-0.1`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-0.1) [`qmmp-0.10`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-0.10) [`qmmp-0.10.1`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-0.10.1) [`qmmp-0.11`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-0.11) [`qmmp-0.12`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-0.12) [`qmmp-0.2`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-0.2) [`qmmp-0.3`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-0.3) [`qmmp-0.4`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-0.4) [`qmmp-0.5`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-0.5) [`qmmp-0.6`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-0.6) [`qmmp-0.7`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-0.7) [`qmmp-0.8`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-0.8) [`qmmp-0.9`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-0.9) [`qmmp-1.0`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-1.0) [`qmmp-1.1`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-1.1) [`qmmp-1.1.1`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-1.1.1) [`qmmp-1.2`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-1.2) [`qmmp-1.3`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-1.3) [`qmmp-1.4`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-1.4) [`qmmp-1.5`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-1.5) [`qmmp-1.6`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-1.6) [`qmmp-1.7`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-1.7) [`qmmp-1.7.3`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-1.7.3) [`qmmp-2.0`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-2.0) [`qmmp-2.1`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-2.1) [`qmmp-2.2`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-2.2) [`qmmp-2.3`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-2.3) [`qmmp-2.4`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-2.4) [`qmmp-plgunin-pack-1.3`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-plgunin-pack-1.3) [`qmmp-plugin-pack-0.10`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-plugin-pack-0.10) [`qmmp-plugin-pack-0.11`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-plugin-pack-0.11) [`qmmp-plugin-pack-0.12`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-plugin-pack-0.12) [`qmmp-plugin-pack-0.6`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-plugin-pack-0.6) [`qmmp-plugin-pack-0.7`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-plugin-pack-0.7) [`qmmp-plugin-pack-0.8`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-plugin-pack-0.8) [`qmmp-plugin-pack-0.9`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-plugin-pack-0.9) [`qmmp-plugin-pack-1.0`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-plugin-pack-1.0) [`qmmp-plugin-pack-1.1`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-plugin-pack-1.1) [`qmmp-plugin-pack-1.1.1`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-plugin-pack-1.1.1) [`qmmp-plugin-pack-1.2`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-plugin-pack-1.2) [`qmmp-plugin-pack-1.3`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-plugin-pack-1.3) [`qmmp-plugin-pack-1.4`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-plugin-pack-1.4) [`qmmp-plugin-pack-1.5`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-plugin-pack-1.5) [`qmmp-plugin-pack-1.6`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-plugin-pack-1.6) [`qmmp-plugin-pack-1.7`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-plugin-pack-1.7) [`qmmp-plugin-pack-2.0`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-plugin-pack-2.0) [`qmmp-plugin-pack-2.1`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-plugin-pack-2.1) [`qmmp-plugin-pack-2.2`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-plugin-pack-2.2) [`qmmp-plugin-pack-2.3`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-plugin-pack-2.3) [`qmmp-plugin-pack-2.4`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-plugin-pack-2.4) [`svn-to-git-sync`](https://github.com/prinsbert/qmmp-dev/tree/svn-to-git-sync) |
| Tags (2) | [`qmmp-2.4.2`](https://github.com/prinsbert/qmmp-dev/releases/tag/qmmp-2.4.2) [`qmmp-2.4.1`](https://github.com/prinsbert/qmmp-dev/releases/tag/qmmp-2.4.1) |

## Description
Qmmp is an audio-player, written with the help of the Qt library. The user interface is similar to [Winamp](https://nl.wikipedia.org/wiki/Winamp) or [XMMS](https://nl.wikipedia.org/wiki/XMMS). Alternative user interfaces are available, for example by downloading `.wsz` files from the [Webamp Skin Museum](https://skins.webamp.org/).

<img width="588" height="570" alt="Main window of qmmp media player with playlist" src="https://github.com/user-attachments/assets/8647b03b-87ac-427c-bfd8-b4f4e56d715c" />


### Features
-    [MPEG1 layer 2/3](https://en.wikipedia.org/wiki/MP3)
-    [Ogg Vorbis](https://en.wikipedia.org/wiki/Vorbis)
-    [Ogg Opus](https://en.wikipedia.org/wiki/Opus_(audio_format))
-    [Native FLAC/Ogg FLAC](https://en.wikipedia.org/wiki/FLAC)
-    [Musepack](https://en.wikipedia.org/wiki/Musepack)
-    [WavPack](https://en.wikipedia.org/wiki/WavPack)
-    tracker modules ([MOD](https://en.wikipedia.org/wiki/MOD_(file_format)), [S3M](https://en.wikipedia.org/wiki/S3M), [Impulse Tracker](https://en.wikipedia.org/wiki/Impulse_Tracker) (.it), [XM](https://en.wikipedia.org/wiki/XM_(file_format)), etc.)
-    [ADTS AAC](https://en.wikipedia.org/wiki/Advanced_Audio_Coding)
-    [CD Audio](https://en.wikipedia.org/wiki/Compact_Disc_Digital_Audio)
-    [WMA](https://en.wikipedia.org/wiki/Windows_Media_Audio), [Monkey's Audio](https://en.wikipedia.org/wiki/Monkey%27s_Audio) and other formats provided by [FFmpeg](https://en.wikipedia.org/wiki/FFmpeg)
-    [PCM WAVE](https://en.wikipedia.org/wiki/Pulse-code_modulation) (.wav) and other formats provided by [libsndfile](https://en.wikipedia.org/wiki/Libsndfile) library
-    [MIDI](https://en.wikipedia.org/wiki/MIDI)
-    [SID](https://en.wikipedia.org/wiki/MOS_Technology_6581)
-    [Chiptune](https://en.wikipedia.org/wiki/Chiptune) formats (AY, GBS, GYM, HES, KSS, NSF, NSFE, SAP, SPC, VGM, VGZ, VTX)
-    [OSS4](https://en.wikipedia.org/wiki/Open_Sound_System) (FreeBSD)
-    [ALSA](https://en.wikipedia.org/wiki/Advanced_Linux_Sound_Architecture) (Linux)
-    [Pulse Audio](https://en.wikipedia.org/wiki/PulseAudio)
-    [JACK](https://en.wikipedia.org/wiki/JACK_Audio_Connection_Kit)
-    [WaveOut](https://en.wikipedia.org/wiki/Windows_legacy_audio_components#Multimedia_Extensions_(MME)) (Win32)
-    [DirectSound](https://en.wikipedia.org/wiki/DirectSound) (Win32)
-    [WASAPI](https://en.wikipedia.org/wiki/Technical_features_new_to_Windows_Vista#Audio_stack_architecture) (Win32)

<img width="849" height="590" alt="Main window of qmmp media player (alternative skin)" src="https://github.com/user-attachments/assets/4bff2267-9e63-488f-8d9e-596c1c0178f1" />
<img width="742" height="706" alt="Audio visualization" src="https://github.com/user-attachments/assets/1619c8c0-00e0-4cb7-bf45-d4ff142adde0" />

