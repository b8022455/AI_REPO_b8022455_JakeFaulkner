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

	DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_Default;


#ifdef _DEBUG
	eflags =  eflags | DirectX::AudioEngine_Debug;
#endif
	mAudioEngine = std::make_unique<DirectX::AudioEngine>(eflags);

}

void AudioSimple::Update(const GameTimer& gt)
{
	if (mAudioEngine->Update())
	{
		//Apply3D effects here?
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
	mSounds[name].second = std::move(mSounds[name].first->CreateInstance());
	mSounds[name].second->Play(loop);
	mSounds[name].second->SetVolume(volume);
	mSounds[name].second->SetPitch(pitch);
	mSounds[name].second->SetPan(pan);
}

void AudioSimple::Suspend()
{
	mAudioEngine->Suspend();
}

void AudioSimple::Resume()
{
	mAudioEngine->Resume();
}

