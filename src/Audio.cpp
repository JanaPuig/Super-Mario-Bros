#include "Audio.h"
#include "Log.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"

Audio::Audio() : Module()
{
    music = NULL;
    name = "audio";
}

// Destructor
Audio::~Audio()
{}

// Called before render is available
bool Audio::Awake()
{
    LOG("Loading Audio Mixer");
    bool ret = true;
    SDL_Init(0);

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
    {
        LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
        active = false;
        ret = false;
    }

    // Load support for the OGG format (other formats can be added if necessary)
    int flags = MIX_INIT_OGG;
    int init = Mix_Init(flags);

    if ((init & flags) != flags)
    {
        LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
        active = false;
        ret = false;
    }

    // Initialize SDL_mixer with 32 channels (or adjust as needed)
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        active = false;
        ret = false;
    }
    else
    {
        // Allocate 32 channels (adjust this number depending on your needs)
        Mix_AllocateChannels(32); // Number of simultaneous channels allowed
    }

    return ret;
}

// Called before quitting
bool Audio::CleanUp()
{
    if (!active)
        return true;

    LOG("Freeing sound FX, closing Mixer and Audio subsystem");

    if (music != NULL)
    {
        Mix_FreeMusic(music);
    }

    for (const auto& fxItem : fx) {
        Mix_FreeChunk(fxItem);
    }
    fx.clear();

    Mix_CloseAudio();
    Mix_Quit();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);

    return true;
}

// Play a music file
bool Audio::PlayMusic(const char* path, float fadeTime)
{
    bool ret = true;

    if (!active)
        return false;

    if (music != NULL)
    {
        if (fadeTime > 0.0f)
        {
            Mix_FadeOutMusic(int(fadeTime * 1000.0f));
        }
        else
        {
            Mix_HaltMusic();
        }

        // this call blocks until fade out is done
        Mix_FreeMusic(music);
    }

    music = Mix_LoadMUS(path);

    if (music == NULL)
    {
        LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
        ret = false;
    }
    else
    {
        Mix_VolumeMusic(MIX_MAX_VOLUME);

        if (fadeTime > 0.0f)
        {
            if (Mix_FadeInMusic(music, -1, (int)(fadeTime * 1000.0f)) < 0)
            {
                LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
                ret = false;
            }
        }
        else
        {
            if (Mix_PlayMusic(music, -1) < 0)
            {
                LOG("Cannot play music %s. Mix_GetError(): %s", path, Mix_GetError());
                ret = false;
            }
        }
    }

    LOG("Successfully playing %s", path);
    return ret;
}

void Audio::StopMusic(float fadeTime) {
    if (active) {
        if (fadeTime > 0.0f) {
            Mix_FadeOutMusic(int(fadeTime * 1000.0f));
        }
        else {
            Mix_HaltMusic();
        }
    }
}

void Audio::PauseMusic() {
    if (active) {
        Mix_PauseMusic();
    }
}

void Audio::ResumeMusic() {
    if (active) {
        Mix_ResumeMusic();
    }
}

// Load WAV
int Audio::LoadFx(const char* path)
{
    int ret = 0;

    if (!active)
        return 0;

    Mix_Chunk* chunk = Mix_LoadWAV(path);

    if (chunk == NULL)
    {
        LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
    }
    else
    {
        fx.push_back(chunk);
        ret = (int)fx.size();
    }

    return ret;
}

// Play WAV with a specific channel
bool Audio::PlayFx(int id, int repeat)
{
    bool ret = false;

    if (!active)
        return false;

    if (id > 0 && id <= fx.size())
    {
        auto fxIt = fx.begin();
        std::advance(fxIt, id - 1);

        // Use Mix_PlayChannel to play sound effect on a specific channel
        int channel = Mix_PlayChannel(-1, *fxIt, repeat);
        if (channel == -1)
        {
            LOG("Error playing sound effect: %s", Mix_GetError());
        }
        else
        {
            ret = true; // Success in playing sound effect
        }
    }

    return ret;
}

// Pause all FX
void Audio::PauseFx()
{
    if (active)
    {
        Mix_Pause(-1); // Pause all channels
        LOG("Paused all FX");
    }
}

// Resume all FX
void Audio::ResumeFx()
{
    if (active)
    {
        Mix_Resume(-1); // Resume all channels
        LOG("Resumed all FX");
    }
}

void Audio::StopFx()
{
    if (active)
    {
        if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
        {
            LOG("Audio not available, unable to stop FX.");
            return; // No intentamos detener efectos si Mix no está listo
        }

        Mix_HaltChannel(-1); // Stop all active channels
        LOG("Stopped all FX");
    }
    else
    {
        LOG("Audio system not active, cannot stop FX");
    }
}
