#include <iostream>

#include "Utils/Debug.hpp"

#include "Sound/SoundManager.hpp"

std::shared_ptr<SoundManager>   SoundManager::_soundManager = nullptr;

SoundManager::SoundManager() :
_result(FMOD_OK),
_system(nullptr),
_channel1(nullptr),
_channel2(nullptr)
{
    for (int i = 0; i < NB_MAX_SOUNDS; i++)
    {
        _sounds[i].free = true;
        _sounds[i].id = i;
        _sounds[i].sound = nullptr;
        _sounds[i].type = DEFAULT_SOUND;
    }
}

SoundManager::~SoundManager() {}

std::shared_ptr<SoundManager>   SoundManager::getInstance()
{
    if (!_soundManager)
    {
        _soundManager = std::make_shared<SoundManager>();
    }
    return _soundManager;
}

bool    SoundManager::initialize()
{
    _result = FMOD::System_Create(&_system);
    if (!errorCheck())
        return (false);

    _result = _system->init(SOUND_MAX_CHANNELS, FMOD_INIT_NORMAL, 0);
    if (!errorCheck())
        return (false);

    return (true);
}

void    SoundManager::update()
{
    _result = _system->update();
    errorCheck();
}

void    SoundManager::shutdown()
{
    for (int i = 0; i < NB_MAX_SOUNDS; i++)
        freeSound(i);
    _result = _system->close();
    errorCheck();
    _result = _system->release();
    errorCheck();
}

bool    SoundManager::errorCheck()
{
    if (_result != FMOD_OK)
    {
        std::cerr << "FMOD error! (" << _result << ") " << FMOD_ErrorString(_result) << std::endl;
        return (false);
    }
    return (true);
}

void    SoundManager::freeSound(unsigned int id)
{
    if (_sounds[id].free == false)
    {
        _sounds[id].free = true;
        _result = _sounds[id].sound->release();
        errorCheck();
    }
}

int     SoundManager::registerSound(const char *name, eSoundType type)
{
    for (int i = 0; i < NB_MAX_SOUNDS; i++)
    {
        if (_sounds[i].free)
        {
            _sounds[i].free = false;
            _sounds[i].type = type;
            if (type == BACKGROUND_SOUND)
            {
                _result = _system->createStream(name, FMOD_LOOP_NORMAL, 0, &_sounds[i].sound);
            }
            else // type == DEFAULT_SOUND
            {
                _result = _system->createSound(name, FMOD_DEFAULT, 0, &_sounds[i].sound);
            }
            errorCheck();
            return (_sounds[i].id);
        }
    }
    return (-1);
}

void    SoundManager::playSound(unsigned int id)
{
    if (_sounds[id].type == BACKGROUND_SOUND)
    {
        _result = _system->playSound(_sounds[id].sound, 0, false, &_channel1);
    }
    else
    {
        _result = _system->playSound(_sounds[id].sound, 0, false, &_channel2);
    }
    errorCheck();
}