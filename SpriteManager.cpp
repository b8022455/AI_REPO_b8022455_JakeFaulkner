#include "SpriteManager.h"
#include "ResourceUploadBatch.h"
#include "DescriptorHeap.h"

//#include "SpriteBatch.h"
#include "Common/d3dUtil.h"
//#include "Common/d3dx12.h"


#define IID_PPV_ARGS(ppType) __uuidof(**(ppType)), IID_PPV_ARGS_Helper(ppType)



#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif


void SpriteManager::Init(ID3D12Device * device, ID3D12CommandQueue* commandQueue, UINT srvDescSize,DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilFormat)
{

	DirectX::ResourceUploadBatch resourceUpload(device);

	resourceUpload.Begin();
	{
		DirectX::RenderTargetState rtState(backBufferFormat,depthStencilFormat);

		DirectX::SpriteBatchPipelineStateDescription sbPipelineDesc(rtState);

		mSpriteBatch = std::make_unique<DirectX::SpriteBatch>(device,resourceUpload, sbPipelineDesc);

		/*{ // D3D12_DESCRIPTOR_HEAP_DESC version
			D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
			srvHeapDesc.NumDescriptors = 1;
			srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			ThrowIfFailed(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSpriteSrvDescriptorHeap)));

			CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor(mSpriteSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
			CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor(mSpriteSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

			mSpriteFont.at(0) = std::make_unique<DirectX::SpriteFont>(
				device, 
				resourceUpload, 
				L"Data/Fonts/Subway_Novella_16.spritefont",
				hCpuDescriptor,
				hGpuDescriptor
				);

		}*/

		 mDescriptorHeap = std::make_unique<DirectX::DescriptorHeap>(device,
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
			1);

		mSpriteFont.at(0) = std::make_unique<DirectX::SpriteFont>(
			device,
			resourceUpload,
			L"Data/Fonts/Subway_Novella_16.spritefont",
			mDescriptorHeap->GetFirstCpuHandle(),
			mDescriptorHeap->GetFirstGpuHandle()
			);

		//todo next
		//mSpriteFont.at(1) = std::make_unique<DirectX::SpriteFont>( .. )

	}

	auto uploadResourceEnd = resourceUpload.End(commandQueue);
	uploadResourceEnd.wait();


}

void SpriteManager::OnResize( D3D12_VIEWPORT & viewport)
{
}

void SpriteManager::Draw(ID3D12GraphicsCommandList * commandList, const D3D12_VIEWPORT& viewport)
{

	mSpriteBatch->SetViewport(viewport);

	auto mdh = mDescriptorHeap->Heap();


	ID3D12DescriptorHeap* descriptorHeaps[] = { mDescriptorHeap->Heap() };
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	mSpriteBatch->Begin(commandList);
	
	//demo
	mSpriteFont.at(0)->DrawString(mSpriteBatch.get(), L"SpriteFontString0", DirectX::XMFLOAT2(1.0f, 1.0f));
	//mSpriteFont.at(1)->DrawString(mSpriteBatch.get(), L"SpriteFontString1",DirectX::XMFLOAT2(1.0f,32.0f));

	mSpriteBatch->End();


	int a = 0;
}

