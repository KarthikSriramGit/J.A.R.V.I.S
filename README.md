# J.A.R.V.I.S.

**Just A Rather Very Intelligent System**

An open-source AI agent that codes and responds the way Tony Stark's butler does—with dry British wit, the JARVIS voice, face recognition, and a hybrid local/cloud coding brain.

## Features

- **"Wake Up, Daddy's Home"** — Face recognition on boot, plays your welcome track, greets you
- **Voice I/O** — Speech-to-text (whisper.cpp), Text-to-speech (Windows SAPI / Piper)
- **Coding Agent** — OpenAI API + local LLM (llama.cpp) with tool calling
- **JARVIS Personality** — British wit, concise, opinionated but deferential
- **Quiet Mode** — Stand down when you ask; wake on command

## Requirements

- Windows 10/11
- NVIDIA GPU (8GB VRAM) for optional local models
- CMake 3.24+
- Visual Studio 2022 (or build tools)
- [vcpkg](https://vcpkg.io/) (or use the bundled bootstrap)

## Quick Start

```powershell
# 1. Clone and setup
git clone https://github.com/KarthikSriramGit/J.A.R.V.I.S.git
cd J.A.R.V.I.S

# 2. Run setup (downloads models, builds with vcpkg)
.\scripts\setup.ps1

# 3. Add your API key (copy example, then edit)
copy config\secrets.example.yaml config\secrets.yaml
# Edit config/secrets.yaml and replace with your OpenAI API key

# 4. Place "Should I Stay or Should I Go.mp3" in assets/audio/should_i_stay.mp3

# 5. Enroll your face
cd build
.\jarvis_enroll.exe owner

# 6. Run J.A.R.V.I.S.
.\jarvis.exe
```

## Build

```powershell
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

## Private Files (Do Not Commit)

The following are in `.gitignore` and will **not** be pushed to GitHub:

- **API key** — Copy `config/secrets.example.yaml` to `config/secrets.yaml`, add your key, and never commit `secrets.yaml`. Or use the `OPENAI_API_KEY` environment variable.
- **Audio files** — `assets/audio/*.mp3` and `*.wav` are ignored. Add your welcome track locally.
- **Face database** — `assets/face_db/*.dat` (enrolled faces)
- **Local overrides** — `config/local.yaml`, `.env`, `.env.local`

## Configuration

Edit `config/jarvis.yaml`:

- `paths.root` — Set to `".."` when running from `build/` to use project-root models
- `llm.cloud_api_key_env` — Environment variable for OpenAI API key (default: `OPENAI_API_KEY`)
- `personality.user_name` — How JARVIS addresses you (default: "sir")

## Project Structure

```
J.A.R.V.I.S/
├── src/
│   ├── core/       # Config, logger, orchestrator, event bus
│   ├── vision/     # Camera, face recognition, enrollment
│   ├── audio/      # Capture, STT, TTS, player
│   ├── brain/      # LLM local/cloud, router, personality
│   ├── agent/      # Tool registry, file ops, shell, code search
│   └── platform/   # Startup sequence, system tray, Windows service
├── assets/         # Prompts, audio, face database
├── config/         # jarvis.yaml
├── models/         # Downloaded models (whisper, llama, TTS, face)
└── scripts/        # setup.ps1, enroll_face.ps1, install_service.ps1
```

## Models

The setup script downloads face recognition models. For full functionality:

- **Whisper**: `ggml-base.en.bin` from [whisper.cpp](https://github.com/ggerganov/whisper.cpp) → `models/whisper/`
- **LLM**: Qwen2.5-7B-Instruct Q4_K_M → `models/llm/`
- **TTS**: Piper `en_GB-alan-medium` from [piper-voices](https://huggingface.co/rhasspy/piper-voices) → `models/tts/`

## License

MIT
