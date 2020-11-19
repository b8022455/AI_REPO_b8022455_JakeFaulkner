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
		//DirectX::SoundEffectInstance_Default |
		DirectX::SoundEffectInstance_Use3D 
		//DirectX::SoundEffectInstance_ReverbUseFilters
		;

	mSounds[name].second = std::move(mSounds[name].first->CreateInstance(iflags));
	
	mSounds[name].second->Play(loop);
	mSounds[name].second->Apply3D(mListener,mEmitter);
	//mSounds[name].second->SetVolume(volume);
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

