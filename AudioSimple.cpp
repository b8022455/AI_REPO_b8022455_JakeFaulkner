#include "AudioSImple.h"
#include "Common/GameTimer.h"
#include <string>

AudioSimple::~AudioSimple()
{
	if (mAudioEngine)
	{
		mAudioEngine->Suspend();
	}
}

void AudioSimple::Init()
{

	//todo put in GameAudio
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		assert(false);
	}


	DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_Default | DirectX::AudioEngine_EnvironmentalReverb | DirectX::AudioEngine_ReverbUseFilters;


#ifdef _DEBUG
	eflags =  eflags | DirectX::AudioEngine_Debug;
#endif
	//Setup engine
	mAudioEngine = std::make_unique<DirectX::AudioEngine>(eflags,nullptr,nullptr, AudioCategory_GameEffects);

	// Cone setup for directional sound
	mCone = X3DAudioDefault_DirectionalCone; //// { X3DAUDIO_PI / 2, X3DAUDIO_PI, 1.0f, 0.708f, 0.0f, 0.25f, 0.708f, 1.0f };
	// Override default directional cone properties
	mCone.InnerAngle = X3DAUDIO_PI / 4;
	mCone.OuterAngle = X3DAUDIO_PI / 2;
	mCone.InnerVolume = 2.0f;
	mCone.OuterVolume = 0.0f; // can't hear outside of OuterAngle at 0.0f
	//Assign cone to listener
	mListener.pCone = &mCone;


}

void AudioSimple::Update(const GameTimer& gt, const DirectX::XMFLOAT3& camPos, const DirectX::XMFLOAT3& camForward, const DirectX::XMFLOAT3& camUp)
{
	if (mAudioEngine->Update())
	{
		//Passes camera properties for 3D sound
		mListener.SetPosition(camPos);
		mListener.SetOrientation(camForward, camUp);

		//emitter example
		mEmitter.SetPosition(DirectX::XMFLOAT3(cosf(gt.TotalTime())*0.0f, 0.0f, 1.0f));

	}
	else
	{
		// No audio device is active
		if (mAudioEngine->IsCriticalError())
		{
			assert(false);
		}
	}
}

void AudioSimple::Load(const std::string& name, const std::wstring& filename)
{
	SoundPair soundPair;
	soundPair.first = std::make_unique<DirectX::SoundEffect>(mAudioEngine.get(), filename.c_str() );
	assert(soundPair.first);
	
	mSounds[name] = std::move(soundPair);
	
}

void AudioSimple::Play(const std::string& name, bool loop, float volume, float pitch , float pan )
{
	assert(mSounds[name].first);

	DirectX::SOUND_EFFECT_INSTANCE_FLAGS iflags =
		DirectX::SoundEffectInstance_Default 
		//|
		//DirectX::SoundEffectInstance_Use3D 
		//|
		//DirectX::SoundEffectInstance_ReverbUseFilters
		;

	mSounds[name].second = std::move(mSounds[name].first->CreateInstance(iflags));

	mSounds[name].second->Play(loop);
	//mSounds[name].second->Apply3D(mListener,mEmitter);
	mSounds[name].second->SetVolume(volume);
	//mSounds[name].second->SetPitch(pitch);
	//mSounds[name].second->SetPan(pan);
}

void AudioSimple::Suspend()
{
	mAudioEngine->Suspend();
}

void AudioSimple::Resume()
{
	mAudioEngine->Resume();
}

void AudioSimple::SetReverbRandom()
{
	mAudioEngine->SetReverb((DirectX::AUDIO_ENGINE_REVERB)(rand()% DirectX::AUDIO_ENGINE_REVERB::Reverb_MAX));
}

SfxEngine::SfxEngine(DirectX::AudioListener* listener)
	:
	SoundEngine(listener)
{
	mCacheLimit = 20;
}

void SfxEngine::Init()
{
	//todo change flags
	DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_Default | DirectX::AudioEngine_EnvironmentalReverb | DirectX::AudioEngine_ReverbUseFilters;

#ifdef _DEBUG
	eflags = eflags | DirectX::AudioEngine_Debug;
#endif

	//Setup engine for sfx
	mAudioEngine = std::make_unique<DirectX::AudioEngine>(eflags, nullptr, nullptr, AudioCategory_GameEffects);

}

void SfxEngine::Update(const GameTimer & gt)
{
	
	if (mAudioEngine->Update())
	{
		mCache.remove_if([](const auto& c) {

			bool b1 = c->GetState() == DirectX::SoundState::STOPPED;
			return c->GetState() == DirectX::SoundState::STOPPED;
		});
	}
	else
	{
		// No audio device is active
		if (mAudioEngine->IsCriticalError())
		{
			assert(false);
		}
	}
	
}

void SfxEngine::Play(const std::string & soundName, bool loop, float volume, float pitch, float pan, DirectX::AudioEmitter * emitter)
{

	assert(mSounds[soundName]);

	DirectX::SOUND_EFFECT_INSTANCE_FLAGS iflags = GetInstanceFlags(emitter);

	mCache.push_front( std::move(mSounds[soundName]->CreateInstance(iflags)));

	mCache.front()->Play(loop);

	if (emitter)
	{
		//3D
		mCache.front()->Apply3D(*pListener,*emitter);
	}
	else
	{
		//Non-3D
		mCache.front()->SetVolume(volume);
		mCache.front()->SetPitch(pitch);
		mCache.front()->SetPan(pan);
	}

}






MusicEngine::MusicEngine(DirectX::AudioListener* listener)
	:
	SoundEngine(listener)
{
	mCacheLimit = 2;
}

void MusicEngine::Init()
{
	DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_Default;

#ifdef _DEBUG
	eflags = eflags | DirectX::AudioEngine_Debug;
#endif
	//Setup engine for music and ambience
	mAudioEngine = std::make_unique<DirectX::AudioEngine>(eflags, nullptr, nullptr, AudioCategory_GameMedia);
	
	assert(mCache.size() == 0);

	std::generate_n(std::back_inserter(mCache), mCacheLimit, []() 
	{
		
		return std::unique_ptr<DirectX::SoundEffectInstance>{};
	});
	
	
	
}

void MusicEngine::Update(const GameTimer & gt)
{
	assert(mCache.size() == mCacheLimit);
	
	if (mNormalisedVolume > 0.5f && mNormalisedVolume < 0.6f)
	{
		int a = 0;
	}

	//Increase volume for cache 
	mNormalisedVolume += gt.DeltaTime() / mFadeInSecs;

	//Limit normalised volume
	if (mNormalisedVolume > 1.0f)
		mNormalisedVolume = 1.0f;
	else if (mNormalisedVolume < 0.0f)
		mNormalisedVolume = 0.0f;

	//Apply volume
	if (mCache.front())
		mCache.front()->SetVolume(mNormalisedVolume);
	if (mCache.back())
		mCache.back()->SetVolume(1.0f - mNormalisedVolume); //compliment

}

void MusicEngine::Play(const std::string & soundName, bool loop, float volume, float pitch, float pan, DirectX::AudioEmitter * emitter)
{


	assert(mSounds[soundName]);

	SwapCache(); //Current front audio swapped with last


	DirectX::SOUND_EFFECT_INSTANCE_FLAGS iflags = GetInstanceFlags(emitter);

	//New instance to first cache element
	mCache.front() = std::move(mSounds[soundName]->CreateInstance(iflags));

	mCache.front()->Play(loop);

	//Set properties
	if (emitter && pListener)
	{
		mCache.front()->Apply3D(*pListener,*emitter);
	}
	else
	{
		mCache.front()->SetVolume(mNormalisedVolume);
		mCache.front()->SetPitch(pitch);
		mCache.front()->SetPan(pan);
	}

}

void MusicEngine::SwapCache()
{
	mCache.front().swap(mCache.back());
	mNormalisedVolume = 1.0f - mNormalisedVolume;//swap audio volume 
}










GameAudio::~GameAudio()
{
	OutputDebugStringA("~GameAudio   \n");
	mEngines.clear();
}

void GameAudio::Init()
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		assert(false);
	}
	mCone = X3DAudioDefault_DirectionalCone;

}

void GameAudio::CreateEngine(const std::string & name, const EngineType & type)
{
	if (!mEngines[name])
	{
		switch (type)
		{
		case ENGINE_SFX:	mEngines[name] = std::make_unique<SfxEngine>(&mListener); break;
		case ENGINE_MUSIC:	mEngines[name] = std::make_unique<MusicEngine>(&mListener); break;
		default:			assert(false);
		}


		mEngines[name]->Init();
	}
	else
	{
		assert(false);
	}
}

void GameAudio::Update(const GameTimer & gt, const DirectX::XMFLOAT3 & camPos, const DirectX::XMFLOAT3 & camForward, const DirectX::XMFLOAT3 & camUp)
{
	//Camera as listener
	mListener.SetPosition(camPos);
	mListener.SetOrientation(camForward, camUp);

	for (auto& engine : mEngines)
	{
		engine.second->Update(gt);
	}
}

void GameAudio::Play(const std::string & soundName, bool loop, float volume, float pitch, float pan, DirectX::AudioEmitter * emitter)
{
	if (mkeys.count(soundName) == 1)
	{
		mEngines[mkeys[soundName]]->Play(soundName, loop,volume,pitch,pan,emitter);
	}
	else
	{
		std::string msg = "Error - Audio cannot play: " + soundName + "\n";
		OutputDebugStringA(msg.c_str());
		assert(false);
	}

	
}

void GameAudio::LoadSound(const std::string & engineName, const std::string & soundName, const std::wstring & filename)
{
	std::string debug = "";


	//Check sound doesnt already exist
	if (mkeys.count(soundName) == 0)
	{
		//Create an SFX engine if one doesn't exist
		if (mEngines.count(engineName) == 0)
		{
			CreateEngine(engineName, ENGINE_SFX);
			OutputDebugStringA("Warning - Engine not created. Default SFX_Engine created");
		}

		//Adds keys to map
		mkeys[soundName] = engineName;

		//Add audio to engine
		mEngines[engineName]->LoadSound(soundName, filename);

		std::string debug = "Success - Audio loaded: " + soundName + "on engine " + engineName + "\n";
	}
	else
	{
		std::string debug = "Warning - Audio already loaded: " + soundName + "on engine " + engineName + "\n";
	}
	
	OutputDebugStringA(debug.c_str());
}

std::string GameAudio::GetVolume(const std::string& engineName)
{

	return mEngines[engineName]->GetVolume().c_str();
}

DirectX::SOUND_EFFECT_INSTANCE_FLAGS SoundEngine::GetInstanceFlags(DirectX::AudioEmitter * emitter)
{
	DirectX::SOUND_EFFECT_INSTANCE_FLAGS iflags = DirectX::SoundEffectInstance_Default;

	//if emitter provided then 3d sound filter applied 
	if (emitter)
		iflags = iflags | DirectX::SoundEffectInstance_Use3D;

	return iflags;
}

SoundEngine::SoundEngine(DirectX::AudioListener* listener)
	:
	pListener(listener)
{

}

void SoundEngine::LoadSound(const std::string & soundName, const std::wstring & filename)
{
	//Loads a sound for the engine
	auto sound = std::make_unique<DirectX::SoundEffect>(mAudioEngine.get(), filename.c_str());
	assert(sound);
	mSounds[soundName] = std::move(sound);
}

std::string SoundEngine::GetVolume()
{
	std::string str;


	str = std::to_string( mNormalisedVolume);


	return str;
}
