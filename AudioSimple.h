#pragma once

#include "Audio.h"
#include <string>

#include <unordered_map>
#include <map>
#include <array>

class GameTimer;

//Simple Audio Demo
class AudioSimple
{
	typedef std::pair< std::unique_ptr<DirectX::SoundEffect>, std::unique_ptr<DirectX::SoundEffectInstance>> SoundPair;

	std::unique_ptr<DirectX::AudioEngine> mAudioEngine;			// Globals for sound effects and instances assigned to it
	std::unordered_map<std::string, SoundPair>  mSounds;		// Sounds assigned a string name

	DirectX::AudioEmitter mEmitter; //todo GameObjects create emitters rather than one member variable
	DirectX::AudioListener mListener; // camera properties passed in through Update()
	X3DAUDIO_CONE mCone;
public:
	~AudioSimple();

	void Init();
	void Update(const GameTimer& gt, const DirectX::XMFLOAT3& camPos , const DirectX::XMFLOAT3& camForward, const DirectX::XMFLOAT3& camUp);
	void Load(const std::string& name, const std::wstring& filename);
	void Play(const std::string& name, bool loop = false, float volume = 1.0f,float pitch = 0.0f,float pan = 0.0f);
	void Suspend();
	void Resume();
	void SetReverbRandom();
};

/*
      SoundEngine
        |    |
    -----    -----
   |              |
SfxEngine   Music Engine

*/

class SoundEngine
{
protected:
	std::unique_ptr<DirectX::AudioEngine> mAudioEngine;
	std::map< std::string, std::unique_ptr<DirectX::SoundEffect>> mSounds;	// Load sounds and assigned to engine
	std::list<std::unique_ptr<DirectX::SoundEffectInstance>> mCache;		//Plays instances of sounds
	UINT mCacheLimit;
	DirectX::AudioListener* pListener;
	float mNormalisedVolume = 1.0f;
	float mFadeInSecs = 3.0f; 
	DirectX::SOUND_EFFECT_INSTANCE_FLAGS GetInstanceFlags(DirectX::AudioEmitter* emitter); //Flags set based on if emitter provided
public:
	SoundEngine(DirectX::AudioListener* listener);
	virtual void Init() = 0;
	virtual void Update(const GameTimer& gt) = 0;
	virtual void Play(const std::string& soundName, bool loop = false, float volume = 1.0f, float pitch = 0.0f, float pan = 0.0f, DirectX::AudioEmitter* emitter = nullptr) = 0;
	void LoadSound(const std::string& soundName, const std::wstring& filename);
	std::string GetVolume();
};

class SfxEngine : public SoundEngine
{
public:
	SfxEngine(DirectX::AudioListener* listener);
	void Init() override;
	void Update(const GameTimer& gt) override;
	void Play(const std::string& soundName, bool loop = false, float volume = 1.0f, float pitch = 0.0f, float pan = 0.0f, DirectX::AudioEmitter* emitter = nullptr) override; // push back mCache
	
};

class MusicEngine : public SoundEngine
{
private:
public:
	MusicEngine(DirectX::AudioListener* listener);
	void Init() override;
	void Update(const GameTimer& gt) override;
	void Play(const std::string& soundName,  bool loop = false, float volume = 1.0f, float pitch = 0.0f, float pan = 0.0f, DirectX::AudioEmitter* emitter = nullptr) override;
private:
	void SwapCache();
};

class GameAudio
{
private:
	std::map<std::string, std::unique_ptr<SoundEngine>> mEngines;
	std::map<std::string, std::string> mkeys; //audioname and enginename
	float masterVolume;
	DirectX::AudioListener mListener; 
	X3DAUDIO_CONE mCone; // for 3D sound. Default set in Init()
public:
	enum EngineType{ENGINE_SFX,ENGINE_MUSIC};

	~GameAudio();
	void Init();
	void CreateEngine(const std::string& name, const EngineType& type);
	void Update(const GameTimer& gt, const DirectX::XMFLOAT3& camPos, const DirectX::XMFLOAT3& camForward, const DirectX::XMFLOAT3& camUp);
	void Play(const std::string& soundName, bool loop = false, float volume = 1.0f, float pitch = 0.0f, float pan = 0.0f, DirectX::AudioEmitter* emitter = nullptr);
	void LoadSound(const std::string& engineName, const std::string& soundName, const std::wstring& filename);
	std::string GetVolume(const std::string& engineName);
};


#include <time.h>
struct SoundTestTimer
{
	float timer = 10.0f;
	
	bool HasTimeElapsed(float dt, float resetOnElapsed)
	{
		timer -= dt;
		bool elapsed(timer <= 0.0f);
		if (elapsed)
			timer = std::abs(resetOnElapsed);

		return elapsed;
	}
};