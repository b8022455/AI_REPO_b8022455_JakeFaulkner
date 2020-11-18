#pragma once

#include "Audio.h"
#include <string>
#include <map>
#include <array>

class GameTimer;

//Simple Audio Demo
class AudioSimple
{
	std::unique_ptr<DirectX::AudioEngine> mAudioEngine;			// Globals for sound effects and instances assigned to it
	std::unique_ptr<DirectX::SoundEffect> mSoundEffect;			//Loads audio from file
	std::unique_ptr<DirectX::SoundEffectInstance> mInstance;	// Assigned from mSoundEffect
public:
	~AudioSimple();

	void Init();
	void Update(const GameTimer& gt);
	void Load(const std::wstring& filename);
	void Play(bool loop = false, float volume = 1.0f,float pitch = 0.0f,float pan = 0.0f);
	void Suspend();
	void Resume();
};

