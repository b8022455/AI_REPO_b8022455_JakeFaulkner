//***************************************************************************************
// InstancingAndCullingApp.cpp by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

#include "GameApp.h"

#include <xaudio2.h>

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	try
	{
		GameApp theApp(hInstance);
		if (!theApp.Initialize())
			return 0;

		return theApp.Run();
	}
	catch (DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
}

GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance)
{
}

GameApp::~GameApp()
{
	OutputDebugStringA("~GameApp before flush \n");

	if (md3dDevice != nullptr)
		FlushCommandQueue();

	OutputDebugStringA("~GameApp after flush \n");
}

bool GameApp::Initialize()
{
	if (!D3DApp::Initialize())
		return false;


	mStateManager.Init();
	mStateManager.AddState("foo", std::make_unique<StateA>());
	mStateManager.AddState("bar", std::make_unique<StateB>());

	mStateManager.ChangeState("bar");
	//mStateManager.RemoveState("foo");


	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	// Get the increment size of a descriptor in this heap type.  This is hardware specific, 
	// so we have to query this information.
	mCbvSrvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//mCamera.SetPosition(0.0f, 2.0f, -15.0f);

	mCamera.SetPosition(0.0f, 50.0f, 0.0f);

	float dy = XMConvertToRadians(90.0f);
	mCamera.Pitch(dy); // SETS CAMERA TO FACE DOWN

	//Audio setup
	{
		mGameAudio.Init();

		// Creates an 'engine' for sound effects. 
		mGameAudio.CreateEngine("sfx", AUDIO_ENGINE_TYPE::SFX);
		// Loads a sound onto an 'engine'
		mGameAudio.LoadSound("sfx", "chord", L"Data/Sounds/chord.wav");
		// Plays up to 20 instances at once. SFX only
		mGameAudio.SetCacheSize("sfx", 20);
		// New instance to play when cache is full. Oldest instance removed SFX only
		mGameAudio.ForceAudio("sfx", true);

		//Music 'engine'
		mGameAudio.CreateEngine("music", AUDIO_ENGINE_TYPE::MUSIC);
		// Loads sounds the same way
		mGameAudio.LoadSound("music", "ring5", L"Data/Sounds/Ring05.wav");
		mGameAudio.LoadSound("music", "ring9", L"Data/Sounds/Ring09.wav");
		// Time it takes to fade between tracks when Play() is called
		mGameAudio.SetFade("music", 3.0f);
		// Plays audio from 'music' engine. No need to specify engine
		mGameAudio.Play("ring9", nullptr, true);
		// Is that better?
		mGameAudio.SetEngineVolume("music", 0.005f);
	}

	mCombatController.Initialize();
	mPlayer.Initialize();

	LoadTextures();
	BuildRootSignature();
	BuildDescriptorHeaps();
	BuildShadersAndInputLayout();
	BuildBoxGeometry();
	BuildSwordGeometry();
	BuildPlayerGeometry();
	BuildMaterials();
	BuildRenderItems();
	BuildFrameResources();
	BuildPSOs();

	// Execute the initialization commands.
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until initialization is complete.
	FlushCommandQueue();

	return true;
}

void GameApp::OnResize()
{
	D3DApp::OnResize();

	mCamera.SetLens(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);

	BoundingFrustum::CreateFromMatrix(mCamFrustum, mCamera.GetProj());
}

void GameApp::Update(const GameTimer& gt)
{
	OnKeyboardInput(gt);

	// Cycle through the circular frame resource array.
	mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % gNumFrameResources;
	mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();

	// Has the GPU finished processing the commands of the current frame resource?
	// If not, wait until the GPU has completed commands up to this fence point.
	if (mCurrFrameResource->Fence != 0 && mFence->GetCompletedValue() < mCurrFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(mFence->SetEventOnCompletion(mCurrFrameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	AnimateMaterials(gt);
	UpdateInstanceData(gt);
	UpdateMaterialBuffer(gt);
	UpdateMainPassCB(gt);

	mStateManager.Update(gt);

	mGameAudio.Update(gt, mCamera.GetPosition3f(), mCamera.GetLook3f(), mCamera.GetUp3f());
	//Music fades every 6 seconds
	if (mAudioTimer.HasTimeElapsed(gt.DeltaTime(), 6.0f))
	{
		bool r = rand() % 2;
		if (r)
			mGameAudio.Play("ring5", nullptr, true);
		else
			mGameAudio.Play("ring9", nullptr, true);
	}

}

void GameApp::Draw(const GameTimer& gt)
{
	auto cmdListAlloc = mCurrFrameResource->CmdListAlloc;

	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(cmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(mCommandList->Reset(cmdListAlloc.Get(), mPSOs["opaque"].Get()));

	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	// Indicate a state transition on the resource usage.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// Clear the back buffer and depth buffer.
	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	ID3D12DescriptorHeap* descriptorHeaps[] = { mSrvDescriptorHeap.Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

	// Bind all the materials used in this scene.  For structured buffers, we can bypass the heap and 
	// set as a root descriptor.
	auto matBuffer = mCurrFrameResource->MaterialBuffer->Resource();
	mCommandList->SetGraphicsRootShaderResourceView(1, matBuffer->GetGPUVirtualAddress());

	auto passCB = mCurrFrameResource->PassCB->Resource();
	mCommandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());

	// Bind all the textures used in this scene.
	mCommandList->SetGraphicsRootDescriptorTable(3, mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	DrawRenderItems(mCommandList.Get(), mOpaqueRitems);

	mStateManager.Draw(gt);

	// Indicate a state transition on the resource usage.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// Done recording commands.
	ThrowIfFailed(mCommandList->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Swap the back and front buffers
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	// Advance the fence value to mark commands up to this fence point.
	mCurrFrameResource->Fence = ++mCurrentFence;

	// Add an instruction to the command queue to set a new fence point. 
	// Because we are on the GPU timeline, the new fence point won't be 
	// set until the GPU finishes processing all the commands prior to this Signal().
	mCommandQueue->Signal(mFence.Get(), mCurrentFence);
}

void GameApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void GameApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void GameApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		mCamera.Pitch(dy);
		mCamera.RotateY(dx);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void GameApp::OnKeyboardInput(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();


	if (GetAsyncKeyState(VK_UP) & 0x8000)
		mCamera.Elevate((5.0f + mPlayer.GetPos(mAllRitems).z) * dt);

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		mCamera.Elevate(-5.0f * dt);

	if (GetAsyncKeyState('I') & 0x8000)
		mCamera.Walk(20.0f * dt);

	if (GetAsyncKeyState('O') & 0x8000)
		mCamera.Walk(-20.0f * dt);

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		mCamera.Strafe(-5.0f * dt);

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		mCamera.Strafe(5.0f * dt);

	if (GetAsyncKeyState('P') & 0x8000)
		mCamera.SetPosition(0.0f, 50.0f, 0.0f);

	if (GetAsyncKeyState('1') & 0x8000)
		mFrustumCullingEnabled = true;

	if (GetAsyncKeyState('2') & 0x8000)
		mFrustumCullingEnabled = false;

	if (GetAsyncKeyState('Q') & 0x08000)
	{
		//mAudio.Play("Chord",false,1.0f/*,sinf(gt.TotalTime()*0.0f)*/);
		mGameAudio.Play("chord", false);
		mGameAudio.Pause("music");
	}
	if (GetAsyncKeyState('E') & 0x08000)
	{
		//mAudio.Play("Chord",false,1.0f/*,sinf(gt.TotalTime()*0.0f)*/);
		//mGameAudio.Play("ring9", true);
		mGameAudio.Resume("music");
	}

	if (GetAsyncKeyState('4') & 0x8000)
	{
		// TODO: WORKING MOVEMENT TEST LOCATED HERE
		XMMATRIX transform = XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslation(5.0f * dt, 0.0f, 0.0f));
		XMMATRIX current = XMLoadFloat4x4(&mAllRitems.at(0)->Instances.at(0).World);
		transform = XMMatrixMultiply(current, transform);
		XMStoreFloat4x4(&mAllRitems.at(0)->Instances.at(0).World, transform);
	}

	//Checks input when attacking
	if (GetAsyncKeyState('V') & 0x8000)		///Change key in future
		mCombatController.PlayerAttack(mAllRitems);


	if (GetAsyncKeyState('Z') & 0x08000)
		mStateManager.ChangeState("foo");
	
	if (GetAsyncKeyState('X') & 0x08000)
		mStateManager.ChangeState("bar");

	mCamera.SetPosition(mPlayer.GetPos(mAllRitems).x, mCamera.GetPosition3f().y, mPlayer.GetPos(mAllRitems).z);

	mPlayer.Move(mAllRitems, gt);
	mCamera.UpdateViewMatrix();
}

void GameApp::AnimateMaterials(const GameTimer& gt)
{

}

void GameApp::UpdateInstanceData(const GameTimer& gt)
{
	XMMATRIX view = mCamera.GetView();
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	
	mCombatController.Update(mAllRitems);		//Continues rotating the weapon if the player has attacked
	mPlayer.UpdatePos(mAllRitems);

	//Checks if weapon is colliding w/ example box
	if (mCombatController.CheckCollision(mAllRitems["Enemy"]->Instances.at(0).World._41, mAllRitems["Enemy"]->Instances.at(0).World._42,
		mAllRitems["Enemy"]->Instances.at(0).World._43))
	{
		mAllRitems["Enemy"]->Instances.at(0).MaterialIndex = 5;			//Visual representation for collision
		enemyHealth -= 5;
		mAllRitems["Enemy"]->Instances.at(0).World._41 += 5.0f;			///Pushes enemy back after being hit by sword, In future have enemy move back based on which way player is facing !!!
	}

	///Enemy Pos, Remove into Enemy class in future!!!
	XMFLOAT3 enemyPos = XMFLOAT3(mAllRitems["Enemy"]->Instances.at(0).World._41, mAllRitems["Enemy"]->Instances.at(0).World._42, mAllRitems["Enemy"]->Instances.at(0).World._43);

	//Interaction stuff
	if (mCombatController.CheckCollision(mPlayer.GetPos(mAllRitems), enemyPos))			//Checks the distance between the player and the enemy objects
	{
	  mPlayer.health -= 5;
	  XMMATRIX transform = XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslation((mPlayer.GetPos(mAllRitems).x - 5.0f), 0.0f, 0.0f));
	  XMMATRIX current = XMLoadFloat4x4(&mAllRitems["Player"]->Instances.at(0).World);
	  transform = XMMatrixMultiply(current, transform);
	  XMStoreFloat4x4(&mAllRitems["Player"]->Instances.at(0).World, transform);
	  mCamera.Strafe(-5.0f * gt.DeltaTime());
	  mCamera.UpdateViewMatrix();
	}

	int i = 0;					//Makes sure each object with a different geo is using a different instance buffer
	for (auto& e : mAllRitems)
	{
		auto currInstanceBuffer = mCurrFrameResource->InstanceBuffer[i].get();
		const auto& instanceData = e.second->Instances;
		i++;		//Increments the instance buffer for the next geo to be rendered
		int visibleInstanceCount = 0;

		for (UINT i = 0; i < (UINT)instanceData.size(); ++i)
		{
			XMMATRIX world = XMLoadFloat4x4(&instanceData[i].World);
			XMMATRIX texTransform = XMLoadFloat4x4(&instanceData[i].TexTransform);

			XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);

			// View space to the object's local space.
			XMMATRIX viewToLocal = XMMatrixMultiply(invView, invWorld);

			// Transform the camera frustum from view space to the object's local space.
			BoundingFrustum localSpaceFrustum;
			mCamFrustum.Transform(localSpaceFrustum, viewToLocal);

			// Perform the box/frustum intersection test in local space.
			if ((localSpaceFrustum.Contains(e.second->Bounds) != DirectX::DISJOINT) || (mFrustumCullingEnabled == false))
			{
				InstanceData data;
				XMStoreFloat4x4(&data.World, XMMatrixTranspose(world));
				XMStoreFloat4x4(&data.TexTransform, XMMatrixTranspose(texTransform));
				data.MaterialIndex = instanceData[i].MaterialIndex;

				// Write the instance data to structured buffer for the visible objects.
				currInstanceBuffer->CopyData(visibleInstanceCount++, data);
			}
		}

		e.second->InstanceCount = visibleInstanceCount;

		std::wostringstream outs;
		outs.precision(2);
		outs << L"Instancing and Culling Demo" <<
			L"    " << e.second->InstanceCount <<
			L" objects visible out of " << e.second->Instances.size();
		mMainWndCaption = outs.str();
	}
}

void GameApp::UpdateMaterialBuffer(const GameTimer& gt)
{
	auto currMaterialBuffer = mCurrFrameResource->MaterialBuffer.get();
	for (auto& e : mMaterials)
	{
		// Only update the cbuffer data if the constants have changed.  If the cbuffer
		// data changes, it needs to be updated for each FrameResource.
		Material* mat = e.second.get();
		if (mat->NumFramesDirty > 0)
		{
			XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

			MaterialData matData;
			matData.DiffuseAlbedo = mat->DiffuseAlbedo;
			matData.FresnelR0 = mat->FresnelR0;
			matData.Roughness = mat->Roughness;
			XMStoreFloat4x4(&matData.MatTransform, XMMatrixTranspose(matTransform));
			matData.DiffuseMapIndex = mat->DiffuseSrvHeapIndex;

			currMaterialBuffer->CopyData(mat->MatCBIndex, matData);

			// Next FrameResource need to be updated too.
			mat->NumFramesDirty--;
		}
	}
}

void GameApp::UpdateMainPassCB(const GameTimer& gt)
{
	XMMATRIX view = mCamera.GetView();
	XMMATRIX proj = mCamera.GetProj();

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	mMainPassCB.EyePosW = mCamera.GetPosition3f();
	mMainPassCB.RenderTargetSize = XMFLOAT2((float)mClientWidth, (float)mClientHeight);
	mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / mClientWidth, 1.0f / mClientHeight);
	mMainPassCB.NearZ = 1.0f;
	mMainPassCB.FarZ = 1000.0f;
	mMainPassCB.TotalTime = gt.TotalTime();
	mMainPassCB.DeltaTime = gt.DeltaTime();
	mMainPassCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };

	mMainPassCB.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };

	//flashing red for low health
	if (mPlayer.health <= 95)
	{
		mMainPassCB.Lights[0].Strength = { sin(gt.TotalTime()) / 2 + 0.5f ,0.0f,0.0f };
	}

	else 
	{
		mMainPassCB.Lights[0].Strength = { 0.8f, 0.8f, 0.8f };
	}


	mMainPassCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
	mMainPassCB.Lights[1].Strength = { 0.4f, 0.4f, 0.4f };
	mMainPassCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
	mMainPassCB.Lights[2].Strength = { 0.2f, 0.2f, 0.2f };

	auto currPassCB = mCurrFrameResource->PassCB.get();
	currPassCB->CopyData(0, mMainPassCB);
}

void GameApp::LoadTexture(const std::string & name, const std::wstring & filename)
{
	auto texture = std::make_unique<Texture>();
	assert(texture);
	texture->Name = name;
	texture->Filename = filename;
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), texture->Filename.c_str(),
		texture->Resource, texture->UploadHeap));

	mTextures[texture->Name] = std::move(texture);
}

void GameApp::LoadTextures()
{

	LoadTexture("bricksTex", L"Data/Textures/bricks.dds");
	LoadTexture("stoneTex", L"Data/Textures/stone.dds");
	LoadTexture("tileTex", L"Data/Textures/tile.dds");
	LoadTexture("crateTex", L"Data/Textures/WoodCrate01.dds");
	LoadTexture("iceTex", L"Data/Textures/ice.dds");
	LoadTexture("grassTex", L"Data/Textures/grass.dds");
	LoadTexture("defaultTex", L"Data/Textures/white1x1.dds");

}

void GameApp::BuildRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 7, 0, 0);

	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[4];

	// Perfomance TIP: Order from most frequent to least frequent.
	slotRootParameter[0].InitAsShaderResourceView(0, 1);
	slotRootParameter[1].InitAsShaderResourceView(1, 1);
	slotRootParameter[2].InitAsConstantBufferView(0);
	slotRootParameter[3].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);

	auto staticSamplers = GetStaticSamplers();

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(4, slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(md3dDevice->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(mRootSignature.GetAddressOf())));
}

void GameApp::BuildDescriptorHeaps()
{
	//
	// Create the SRV heap.
	//
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 7;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSrvDescriptorHeap)));

	//
	// Fill out the heap with actual descriptors.
	//
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(mSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	auto bricksTex = mTextures["bricksTex"]->Resource;
	auto stoneTex = mTextures["stoneTex"]->Resource;
	auto tileTex = mTextures["tileTex"]->Resource;
	auto crateTex = mTextures["crateTex"]->Resource;
	auto iceTex = mTextures["iceTex"]->Resource;
	auto grassTex = mTextures["grassTex"]->Resource;
	auto defaultTex = mTextures["defaultTex"]->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = bricksTex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = bricksTex->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	md3dDevice->CreateShaderResourceView(bricksTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, mCbvSrvDescriptorSize);

	srvDesc.Format = stoneTex->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = stoneTex->GetDesc().MipLevels;
	md3dDevice->CreateShaderResourceView(stoneTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, mCbvSrvDescriptorSize);

	srvDesc.Format = tileTex->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = tileTex->GetDesc().MipLevels;
	md3dDevice->CreateShaderResourceView(tileTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, mCbvSrvDescriptorSize);

	srvDesc.Format = crateTex->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = crateTex->GetDesc().MipLevels;
	md3dDevice->CreateShaderResourceView(crateTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, mCbvSrvDescriptorSize);

	srvDesc.Format = iceTex->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = iceTex->GetDesc().MipLevels;
	md3dDevice->CreateShaderResourceView(iceTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, mCbvSrvDescriptorSize);

	srvDesc.Format = grassTex->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = grassTex->GetDesc().MipLevels;
	md3dDevice->CreateShaderResourceView(grassTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, mCbvSrvDescriptorSize);

	srvDesc.Format = defaultTex->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = defaultTex->GetDesc().MipLevels;
	md3dDevice->CreateShaderResourceView(defaultTex.Get(), &srvDesc, hDescriptor);
}

void GameApp::BuildShadersAndInputLayout()
{
	const D3D_SHADER_MACRO alphaTestDefines[] =
	{
		"ALPHA_TEST", "1",
		NULL, NULL
	};

	mShaders["standardVS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["opaquePS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "PS", "ps_5_1");

	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}

void GameApp::BuildBoxGeometry()
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(1.0f, 1.0f, 1.0f, 3);

	std::vector<Vertex> vertices(box.Vertices.size());
	for (size_t i = 0; i < box.Vertices.size(); ++i)
	{
		auto& p = box.Vertices[i].Position;
		vertices[i].Pos = p;
		vertices[i].Normal = box.Vertices[i].Normal;
		vertices[i].TexC = box.Vertices[i].TexC;
	}

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);

	std::vector<std::uint16_t> indices = box.GetIndices16();
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "boxGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs["box"] = submesh;

	mGeometries["boxGeo"] = std::move(geo);
}

void GameApp::BuildSwordGeometry()
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData sword = geoGen.CreateSword(1.0f, 1.0f, 1.0f, 3);

	std::vector<Vertex> vertices(sword.Vertices.size());
	for (size_t i = 0; i < sword.Vertices.size(); ++i)
	{
		auto& p = sword.Vertices[i].Position;
		vertices[i].Pos = p;
		vertices[i].Normal = sword.Vertices[i].Normal;
		vertices[i].TexC = sword.Vertices[i].TexC;
	}

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);

	std::vector<std::uint16_t> indices = sword.GetIndices16();
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "swordGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs["swordGeo"] = submesh;

	mGeometries["swordGeo"] = std::move(geo);
}

void GameApp::BuildPlayerGeometry()
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData player = geoGen.CreatePlayer(6.0f, 6.0f, 6.0f, 3);

	std::vector<Vertex> vertices(player.Vertices.size());
	for (size_t i = 0; i < player.Vertices.size(); ++i)
	{
		auto& p = player.Vertices[i].Position;
		vertices[i].Pos = p;
		vertices[i].Normal = player.Vertices[i].Normal;
		vertices[i].TexC = player.Vertices[i].TexC;
	}

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);

	std::vector<std::uint16_t> indices = player.GetIndices16();
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "playerGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs["playerGeo"] = submesh;

	mGeometries["playerGeo"] = std::move(geo);
}



void GameApp::BuildPSOs()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;

	//
	// PSO for opaque objects.
	//
	ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	opaquePsoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
	opaquePsoDesc.pRootSignature = mRootSignature.Get();
	opaquePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaders["standardVS"]->GetBufferPointer()),
		mShaders["standardVS"]->GetBufferSize()
	};
	opaquePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaders["opaquePS"]->GetBufferPointer()),
		mShaders["opaquePS"]->GetBufferSize()
	};
	opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaquePsoDesc.SampleMask = UINT_MAX;
	opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaquePsoDesc.NumRenderTargets = 1;
	opaquePsoDesc.RTVFormats[0] = mBackBufferFormat;
	opaquePsoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	opaquePsoDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	opaquePsoDesc.DSVFormat = mDepthStencilFormat;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&mPSOs["opaque"])));
}

void GameApp::BuildFrameResources()
{
	for (int i = 0; i < gNumFrameResources; ++i)
	{
		mFrameResources.push_back(std::make_unique<FrameResource>(md3dDevice.Get(),
			1, mInstanceCount, (UINT)mMaterials.size()));
	}
}

void GameApp::BuildMaterial(int& matCBIndex, int texSrvHeapIndex, const std::string & name, float roughness, const DirectX::XMFLOAT4 & diffuseAlbedo, const DirectX::XMFLOAT3 & fresnel)
{

	auto material = std::make_unique<Material>();
	material->Name = name;
	material->MatCBIndex = matCBIndex;
	material->DiffuseSrvHeapIndex = texSrvHeapIndex;
	material->DiffuseAlbedo = diffuseAlbedo;
	material->FresnelR0 = fresnel;
	material->Roughness = roughness;

	mMaterials[material->Name] = std::move(material);

	++matCBIndex; //increments for next material

}

void GameApp::BuildMaterials()
{
	int matIndex = 0;

	BuildMaterial(matIndex, matIndex, "bricks0", 0.1f, { 1.0f,1.0f,1.0f,1.0f }, { 0.02f, 0.02f, 0.02f });
	BuildMaterial(matIndex, matIndex, "stone0", 0.3f);
	BuildMaterial(matIndex, matIndex, "tile0", 0.3f, { 1.0f,1.0f,1.0f,1.0f }, { 0.02f, 0.02f, 0.02f });
	BuildMaterial(matIndex, matIndex, "checkboard0", 0.2f);
	BuildMaterial(matIndex, matIndex, "ice0", 0.0f, { 1.0f,1.0f,1.0f,1.0f }, { 0.01f, 0.01f, 0.01f });
	BuildMaterial(matIndex, matIndex, "grass0", 0.0f, { 1.0f,1.0f,1.0f,1.0f });
	BuildMaterial(matIndex, matIndex, "skullMat");

}

void GameApp::BuildRenderItems()
{
	// Generate instance data.

	auto boxRitem = std::make_unique<RenderItem>();
	boxRitem->World = MathHelper::Identity4x4();
	boxRitem->TexTransform = MathHelper::Identity4x4();
	boxRitem->ObjCBIndex = 0;
	boxRitem->Mat = mMaterials["tile0"].get();
	boxRitem->Geo = mGeometries["boxGeo"].get();
	boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->InstanceCount = 0;
	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
	boxRitem->Bounds = boxRitem->Geo->DrawArgs["box"].Bounds;

	const int n = 32; // USED TO CHOOSE SIZE OF GRID
	mInstanceCount = n * n;
	boxRitem->Instances.resize(mInstanceCount);

	float width = 32.0f;
	float height = 32.0f;
	float depth = 32.0f;
	float x = -0.5f*width;
	float z = -0.5f*depth;
	float dx = width / (n - 1);
	float dz = depth / (n - 1);
	for (int k = 0; k < n; ++k)
	{
		for (int j = 0; j < n; ++j)
		{
			int index = k * n + j;
			// Position instanced along a 3D grid.
			boxRitem->Instances[index].World = XMFLOAT4X4(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				x + j * dx, 0.0f, z + k * dz, 1.0f);

			XMStoreFloat4x4(&boxRitem->Instances[index].TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
			//boxRitem->Instances[index].MaterialIndex = index % mMaterials.size();
			boxRitem->Instances[index].MaterialIndex = 1;
			// not sure of syntax to pull [1] index from this array so cheaty way to get this
		}
	}

	///Generic box used as the weapon default for now
	auto swordRitem = std::make_unique<RenderItem>();
	swordRitem->World = MathHelper::Identity4x4();
	swordRitem->ObjCBIndex = 1;
	swordRitem->InstanceCount = 0;
	swordRitem->Mat = mMaterials["tile0"].get();
	swordRitem->Geo = mGeometries["swordGeo"].get();
	swordRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	swordRitem->IndexCount = swordRitem->Geo->DrawArgs["swordGeo"].IndexCount;
	swordRitem->StartIndexLocation = swordRitem->Geo->DrawArgs["swordGeo"].StartIndexLocation;
	swordRitem->BaseVertexLocation = swordRitem->Geo->DrawArgs["swordGeo"].BaseVertexLocation;
	swordRitem->Instances.resize(1);

	auto playerRitem = std::make_unique<RenderItem>();
	playerRitem->World = MathHelper::Identity4x4();
	playerRitem->ObjCBIndex = 2;
	playerRitem->InstanceCount = 0;
	playerRitem->Mat = mMaterials["ice0"].get();
	playerRitem->Geo = mGeometries["playerGeo"].get();
	playerRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	playerRitem->IndexCount = playerRitem->Geo->DrawArgs["playerGeo"].IndexCount;
	playerRitem->StartIndexLocation = playerRitem->Geo->DrawArgs["playerGeo"].StartIndexLocation;
	playerRitem->BaseVertexLocation = playerRitem->Geo->DrawArgs["playerGeo"].BaseVertexLocation;
	playerRitem->Instances.resize(1);

	auto enemyRitem = std::make_unique<RenderItem>();
	enemyRitem->World = MathHelper::Identity4x4();
	enemyRitem->ObjCBIndex = 3;
	enemyRitem->InstanceCount = 0;
	enemyRitem->Mat = mMaterials["ice0"].get();
	enemyRitem->Geo = mGeometries["boxGeo"].get();
	enemyRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	enemyRitem->IndexCount = enemyRitem->Geo->DrawArgs["box"].IndexCount;				//Just for testing, will give enemy its own geo eventually
	enemyRitem->StartIndexLocation = enemyRitem->Geo->DrawArgs["box"].StartIndexLocation;
	enemyRitem->BaseVertexLocation = enemyRitem->Geo->DrawArgs["box"].BaseVertexLocation;
	enemyRitem->Instances.resize(1);

	mAllRitems["Tiles"] = std::move(boxRitem);
	mAllRitems["Weapon"] = std::move(swordRitem);
	mAllRitems["Player"] = std::move(playerRitem);
	mAllRitems["Enemy"] = std::move(enemyRitem);

	//Uncomment this if testing weapon collision
	#pragma region Weapon Collision Checking

	mAllRitems["Enemy"]->Instances.at(0).World = 
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 5.0f, 1.0f
	};
	mAllRitems["Enemy"]->Instances.at(0).MaterialIndex = 3;

	#pragma endregion

	// All the render items are opaque.
	for (auto& e : mAllRitems)
		mOpaqueRitems.push_back(e.second.get());
}

void GameApp::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems)
{
	// For each render item...
	for (size_t i = 0; i < ritems.size(); ++i)
	{
		auto ri = ritems[i];

		cmdList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
		cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
		cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

		// Set the instance buffer to use for this render-item.  For structured buffers, we can bypass 
		// the heap and set as a root descriptor.
		auto instanceBuffer = mCurrFrameResource->InstanceBuffer[i]->Resource();
		mCommandList->SetGraphicsRootShaderResourceView(0, instanceBuffer->GetGPUVirtualAddress());

		cmdList->DrawIndexedInstanced(ri->IndexCount, ri->InstanceCount, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GameApp::GetStaticSamplers()
{
	// Applications usually only need a handful of samplers.  So just define them all up front
	// and keep them available as part of the root signature.  

	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp };
}