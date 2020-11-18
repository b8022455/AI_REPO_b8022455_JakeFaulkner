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
		//mInstance->SetPitch(sinf(gt.TotalTime()));
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

void AudioSimple::Load(const std::wstring& filename)
{

	mSoundEffect = std::make_unique<DirectX::SoundEffect>(mAudioEngine.get(), filename.c_str() );

	assert(mSoundEffect);
}

void AudioSimple::Play(bool loop, float volume, float pitch , float pan )
{
	mInstance = std::move(mSoundEffect->CreateInstance()); // restarts if called while playing
	assert(mInstance);

	mInstance->Play(loop);
	mInstance->SetVolume(volume);
	mInstance->SetPitch(pitch);
	mInstance->SetPan(pan);
}

void AudioSimple::Suspend()
{
	mAudioEngine->Suspend();
}

void AudioSimple::Resume()
{
	mAudioEngine->Resume();
}

