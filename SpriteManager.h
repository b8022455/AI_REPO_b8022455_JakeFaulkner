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
#include <GraphicsMemory.h>
#include <d3d12.h>


struct Sprite
{
	std::string textureName; //lookup gpu handle
	D3D12_GPU_DESCRIPTOR_HANDLE texture;
	DirectX::XMUINT2 textureSize = DirectX::XMUINT2(128,128);
	RECT destinationRectangle = {0,0,128,128};
	RECT sourceRectangle = {0,0,128,128 };   //todo array for animations?
	DirectX::FXMVECTOR color = DirectX::Colors::White;
	float rotation = 0.0f;
	DirectX::XMFLOAT2 origin = DirectX::XMFLOAT2(0.0f, 0.0f);
	DirectX::SpriteEffects effects = DirectX::SpriteEffects::SpriteEffects_None;
	
	Sprite() {};
	Sprite(const std::string& textureName);

	void operator=(const Sprite& s)
	{
		textureName = s.textureName;
		textureSize = s.textureSize;
		texture = s.texture;
		destinationRectangle = s.destinationRectangle;
		sourceRectangle = s.sourceRectangle;
		rotation = s.rotation;
		origin = s.origin;
		effects = s.effects;
		//color = s.color;
	}

	// Give a valid texture name
	void Initialise(const std::string& textureName);

	void Draw();
};

class Button
{
	enum Action {
		NO_ACTION,
		GOTO_MAIN_MENU,
		GOTO_GAME
	};
	Sprite sprite;
	std::string text;
	Action action;
	bool enabled = true;

public:
	Button() {};
	Button(const Sprite& s, const std::string& t, const Action& a);
	void SetPos(const DirectX::XMFLOAT2& pos);

	void Activate();
};

// Handles fonts and viewport sprites
class SpriteManager
{
	std::unique_ptr<DirectX::GraphicsMemory> mGraphicsMemory;
	std::unique_ptr<DirectX::SpriteBatch> mSpriteBatch;
	std::array<std::unique_ptr<DirectX::SpriteFont>, FONT_SIZE> mSpriteFont;

public:

	SpriteManager() {};
	void Init(ID3D12Device* device, ID3D12CommandQueue* commandQueue, UINT srvDescSize, DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilFormat, CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle, CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle);
	void CreateFontResource(ID3D12Device* device, ID3D12CommandQueue* commandQueue, UINT srvDescSize, DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilFormat);
	void OnResize( D3D12_VIEWPORT& viewport);
	// Called in GameApp::Draw method before drawing sprites and fonts
	void DrawBegin(ID3D12GraphicsCommandList* commandList, const D3D12_VIEWPORT& veiwport);
	// Called in GameApp::Draw method after drawing sprites and fonts
	void DrawEnd();
	// Called in State::Draw method. Begin and End methods are called in GameApp::Draw
	void DrawSprite(const std::string& textureName);
	// Called in State::Draw method. Begin and End methods are called in GameApp::Draw
	void DrawSprite(const Sprite& sprite);
	// Called in State::Draw method. Begin and End methods are called in GameApp::Draw
	void DrawFont(size_t i, const std::string& output);
};

