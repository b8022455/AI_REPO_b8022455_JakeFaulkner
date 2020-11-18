#pragma once

#include "Audio.h"
#include <string>
#include <map>
#include <array>

class GameTimer;

//Simple Audio Demo
class AudioSimple
{
	typedef std::pair< std::unique_ptr<DirectX::SoundEffect>, std::unique_ptr<DirectX::SoundEffectInstance>> SoundPair;

	std::unique_ptr<DirectX::AudioEngine> mAudioEngine;			// Globals for sound effects and instances assigned to it
	std::map<std::string, SoundPair>  mSounds;					//Sounds assigned a string name
public:
	~AudioSimple();

	void Init();
	void Update(const GameTimer& gt);
	void Load(const std::string& name, const std::wstring& filename);
	void Play(const std::string& name, bool loop = false, float volume = 1.0f,float pitch = 0.0f,float pan = 0.0f);
	void Suspend();
	void Resume();
};

