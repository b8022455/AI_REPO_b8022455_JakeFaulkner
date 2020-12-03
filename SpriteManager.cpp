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

	graphicsMemory = std::make_unique<DirectX::GraphicsMemory>(device);

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

void SpriteManager::Draw(ID3D12GraphicsCommandList * commandList, const D3D12_VIEWPORT& viewport)
{

	mSpriteBatch->SetViewport(viewport);

	mSpriteBatch->Begin(commandList);
	

	
	//demo
	//mSpriteFont.at(0)->DrawString(mSpriteBatch.get(), L"SpriteFontString0", DirectX::XMFLOAT2(1.0f, 1.0f));
	mSpriteFont.at(0)->DrawString(mSpriteBatch.get(), GameApp::Get().mDebugLog.str().c_str(), DirectX::XMFLOAT2(1.0f, 1.0f));
	//mSpriteFont.at(1)->DrawString(mSpriteBatch.get(), L"SpriteFontString1",DirectX::XMFLOAT2(1.0f,32.0f));

	
	mSpriteBatch->End();

	// clears stream
	GameApp::Get().mDebugLog.str(std::string());

}

void SpriteManager::DrawEnd()
{
}

