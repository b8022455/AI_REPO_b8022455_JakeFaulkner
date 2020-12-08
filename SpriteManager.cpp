#include "SpriteManager.h"
#include "ResourceUploadBatch.h"
#include "DescriptorHeap.h"

#include "Common/d3dUtil.h"
#include "Common/d3dx12.h"
#include "GameApp.h"
#define IID_PPV_ARGS(ppType) __uuidof(**(ppType)), IID_PPV_ARGS_Helper(ppType)



#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

void SpriteManager::Init(ID3D12Device * device, ID3D12CommandQueue* commandQueue, UINT srvDescSize,DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilFormat, CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle, CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle)
{

	mGraphicsMemory = std::make_unique<DirectX::GraphicsMemory>(device);

	DirectX::ResourceUploadBatch resourceUpload(device);

	resourceUpload.Begin();
	{
		DirectX::RenderTargetState rtState(backBufferFormat,depthStencilFormat);
		DirectX::SpriteBatchPipelineStateDescription sbPipelineDesc(rtState);

		mSpriteBatch = std::make_unique<DirectX::SpriteBatch>(device,resourceUpload, sbPipelineDesc);

		mSpriteFont.at(0) = std::make_unique<DirectX::SpriteFont>(
			device,
			resourceUpload,
			L"Data/Fonts/Subway_Novella_16.spritefont",
			cpuHandle,
			gpuHandle
			);

		//todo next
		//mSpriteFont.at(1) = std::make_unique<DirectX::SpriteFont>( .. )

	}

	auto uploadResourceEnd = resourceUpload.End(commandQueue);
	uploadResourceEnd.wait();


}

void SpriteManager::CreateFontResource(ID3D12Device * device, ID3D12CommandQueue * commandQueue, UINT srvDescSize, DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilFormat)
{
}

void SpriteManager::OnResize( D3D12_VIEWPORT & viewport)
{
}

void SpriteManager::DrawBegin(ID3D12GraphicsCommandList * commandList, const D3D12_VIEWPORT & viewport)
{
	mSpriteBatch->SetViewport(viewport);
	mSpriteBatch->Begin(commandList);
}

void SpriteManager::DrawEnd()
{
	// Prints debug string
	mSpriteFont.at(0)->DrawString(mSpriteBatch.get(), GameApp::Get().mDebugLog.str().c_str(), DirectX::XMFLOAT2(1.0f, 1.0f));

	//Ends spritebatch
	mSpriteBatch->End();

	// clears stream
	GameApp::Get().mDebugLog.str(std::string());
}

void SpriteManager::DrawSprite(const std::string & textureName)
{
	mSpriteBatch->Draw(GameApp::Get().GetSpriteGpuDescHandle(textureName), XMUINT2(500, 50), XMFLOAT2(500.0f, 50.0f),  { 1.0f,1.0f,1.0f,1.0f });
}

void SpriteManager::DrawSprite(const Sprite & s)
{
	mSpriteBatch->Draw(
		s.texture, 
		s.textureSize, 
		s.destinationRectangle, 
		&s.sourceRectangle, 
		s.color, 
		s.rotation, 
		s.origin, 
		s.effects
	);
}

void SpriteManager::DrawFont(size_t i, const std::string & output)
{
	if (i < mSpriteFont.size())
	{
		mSpriteFont.at(0)->DrawString(mSpriteBatch.get(), GameApp::Get().mDebugLog.str().c_str(), DirectX::XMFLOAT2(1.0f, 1.0f));
	}
	else
	{
		// i out of range
		assert(false);
	}

}

Sprite::Sprite(const std::string & textureName)
{
	Initialise(textureName);
}

void Sprite::Initialise(const std::string & textureName)
{
	texture = GameApp::Get().GetSpriteGpuDescHandle(textureName);
}

void Sprite::Draw()
{
	GameApp::Get().DrawSprite(*this);
}
