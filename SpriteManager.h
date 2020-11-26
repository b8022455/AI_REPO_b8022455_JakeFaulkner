#pragma once
#include <SpriteFont.h>
#include <array>

#ifndef FONT_SIZE
#define FONT_SIZE 1
#endif // !FONT_SIZE
#include "DescriptorHeap.h"

#include <wrl.h>
#include "TextConsole.h"
#include "Common/d3dx12.h"


// Handles fonts and viewport sprites
class SpriteManager
{
	std::unique_ptr<DirectX::SpriteBatch> mSpriteBatch;
	std::array<std::unique_ptr<DirectX::SpriteFont>, FONT_SIZE> mSpriteFont;
	//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSpriteSrvDescriptorHeap = nullptr;
	//std::unique_ptr<DirectX::DescriptorHeap> mDescriptorHeap = nullptr;

public:
	void Init2();
	void Draw2();

	SpriteManager() {};
	void Init(ID3D12Device* device, ID3D12CommandQueue* commandQueue, UINT srvDescSize, DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilFormat, CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle, CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle);
	void CreateFontResource(ID3D12Device* device, ID3D12CommandQueue* commandQueue, UINT srvDescSize, DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilFormat);
	void OnResize( D3D12_VIEWPORT& viewport);
	void Draw(ID3D12GraphicsCommandList* commandList, const D3D12_VIEWPORT& veiwport);
	void DrawEnd();
};

