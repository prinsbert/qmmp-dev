# qmmp (Qt-based Multimedia Player)
This repostory tracks upstream [qmmp](https://sourceforge.net/projects/qmmp-dev/) and converts the history from SVN to Git.

> ⚠️ **You may be interested in:** a [fork of Qmmp with improvements to the media library](https://github.com/desertmilk/qmmp-with-media-library). It adds Artist/Album filtering to the Media Library, and has better support for [Winamp/Webamp skins](https://skins.webamp.org/) (and behaviors closer to classic Winamp, such as snapping to other panels, etc.).
> 
> <a href="https://github.com/desertmilk/qmmp-with-media-library"><img width="743" height="889" alt="qmmp media player with media library" src="https://github.com/user-attachments/assets/dc9be5fe-37bf-4d52-9045-0ae7a2e7ed5c" /></a>

## SVN → Git mirror

**This repository is an automated conversion of the development history of Qmmp from SVN to Git. It is not the primary source of development for Qmmp.** 

This repository is an archive which tracks the [Qmmp source code](https://sourceforge.net/p/qmmp-dev/) hosted on SourceForge, but with the entire history converted from SVN revisions into Git commits. All real development happens upstream in the [SVN repository](https://sourceforge.net/p/qmmp-dev/code/). The sync script in this repository attempts to follow all [branches](https://github.com/prinsbert/qmmp-dev/branches) as closely as possible, where `trunk` is renamed to [`master`](https://github.com/prinsbert/qmmp-dev/tree/master).

<img width="548" height="430" alt="Main window of qmmp media player" src="https://github.com/user-attachments/assets/a9fe56ba-fc13-43b3-bb42-1162606ff939" />

### Mirror Status

| Field&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | Value |
|:---|:---|
| Latest SVN revision | [`r13335`](https://github.com/prinsbert/qmmp-dev/commit/fb51d7140299bd996ff0b9adcb269670b43ac670) |
| Last changed | 2026-11-04 20:51:22 UTC |
| Branches | [54](https://github.com/prinsbert/qmmp-dev/branches) |
| Tags (3) | [`qmmp-2.4.3`](https://github.com/prinsbert/qmmp-dev/releases/tag/qmmp-2.4.3) [`qmmp-2.4.2`](https://github.com/prinsbert/qmmp-dev/releases/tag/qmmp-2.4.2) [`qmmp-2.4.1`](https://github.com/prinsbert/qmmp-dev/releases/tag/qmmp-2.4.1) |
#### Recent commits (last 10)

| Revision&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | Date&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | Message |
|:---|:---|:---|
| **[`master`](https://github.com/prinsbert/qmmp-dev/tree/master)**/[`r13335`](https://github.com/prinsbert/qmmp-dev/commit/fb51d7140299bd996ff0b9adcb269670b43ac670) | 2026-11-04 17:31 | history: feature to select multiple records |
| [`qmmp-0.12`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-0.12)/[`r13334`](https://github.com/prinsbert/qmmp-dev/commit/4ca89a4cfbb7516b50ade0d2eaeb8a66a43aeac6) | 2026-11-04 13:44 | version bump |
| [`qmmp-0.12`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-0.12)/[`r13333`](https://github.com/prinsbert/qmmp-dev/commit/2e41057ca3f4f134d2d194e1f60b383a210f0b5a) | 2026-11-04 13:25 | gme: fixed fade out setting (#1245) |
| [`qmmp-1.7`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-1.7)/[`r13332`](https://github.com/prinsbert/qmmp-dev/commit/0b63aee3dbdb1b12830382474ce1b35963daaca8) | 2026-11-04 13:21 | gme: fixed fade out setting (#1245) |
| [`qmmp-2.4`](https://github.com/prinsbert/qmmp-dev/tree/qmmp-2.4)/[`r13331`](https://github.com/prinsbert/qmmp-dev/commit/d16623b87582b03fa84de64541d6f12a229d5b5e) | 2026-11-04 13:19 | gme: fixed fade out setting (#1245) |
| **[`master`](https://github.com/prinsbert/qmmp-dev/tree/master)**/[`r13330`](https://github.com/prinsbert/qmmp-dev/commit/90a122acd03aa6fd55c8b6f2c0cc45d1e8b0f768) | 2026-11-04 13:18 | gme: optimization |
| **[`master`](https://github.com/prinsbert/qmmp-dev/tree/master)**/[`r13329`](https://github.com/prinsbert/qmmp-dev/commit/c5b5ed020ad93ee1ff430aceb984cd70b79ce1b7) | 2026-11-04 13:11 | optimization |
| **[`master`](https://github.com/prinsbert/qmmp-dev/tree/master)**/[`r13328`](https://github.com/prinsbert/qmmp-dev/commit/ceaed37f5f8bbd1b84e5c2413536cd7144e75ab5) | 2026-11-04 13:09 | gme: fixed fade out setting (#1245) |
| **[`master`](https://github.com/prinsbert/qmmp-dev/tree/master)**/[`r13327`](https://github.com/prinsbert/qmmp-dev/commit/971a4226f2e95f2bbce0fd4bab1514cc2ac9f04c) | 2026-11-04 10:35 | fixed cppcheck warnings |
| **[`master`](https://github.com/prinsbert/qmmp-dev/tree/master)**/[`r13326`](https://github.com/prinsbert/qmmp-dev/commit/ba230ed3b4986cd4aec45fa0c5a845a528f97faa) | 2026-11-04 10:17 | fixed some cppcheck warnings |
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

