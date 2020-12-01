#pragma once

#define no_init_all // Removes VS 2017 error message. Bug fixed in VS 2019.

#include "Common/d3dApp.h"
#include "Common/MathHelper.h"
#include "Common/UploadBuffer.h"
#include "Common/GeometryGenerator.h"
#include "Common/Camera.h"
#include "FrameResource.h"
#include "AudioSImple.h"
#include "RenderItem.h"
#include "CombatController.h"
#include "SpriteManager.h"
#include "TextConsole.h"
#include "Player.h"
#include "State.h"


class GameApp : public D3DApp, public Singleton<GameApp>
{
public:
	GameApp(HINSTANCE hInstance);
	GameApp(const GameApp& rhs) = delete;
	GameApp& operator=(const GameApp& rhs) = delete;
	~GameApp();

	virtual bool Initialize()override;

	static bool DEBUG;

	const float RIGHTBOUND = 15.0f;
	const float LEFTBOUND = -15.0f;

	const float YBOUNDLOW = 50.0f;
	const float YBOUNDHIGH = 300.0f;

	const float UPBOUND = 15.0f;
	const float DOWNBOUND = -15.0f;

	// Access to instance data from states and gameobjects
	InstanceData* AddRenderItemInstance(const std::string & renderItemName);
	GameApp& GetGameApp() { return *this; }

private:
	virtual void OnResize()override;
	virtual void Update(const GameTimer& gt)override;
	virtual void Draw(const GameTimer& gt)override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

	void OnKeyboardInput(const GameTimer& gt);
	void AnimateMaterials(const GameTimer& gt);
	void UpdateInstanceData(const GameTimer& gt);
	void UpdateMaterialBuffer(const GameTimer& gt);
	void UpdateMainPassCB(const GameTimer& gt);

	void LoadTexture(const std::string& name, const std::wstring& filename);
	void LoadTextures();
	void BuildRootSignature();
	void BuildDescriptorHeaps();
	void BuildShadersAndInputLayout();
	void BuildBoxGeometry();
	void BuildObjGeometry(const std::string& filepath, const std::string& geoName,const std::string& subMeshName);
	void BuildSwordGeometry();
	void BuildPlayerGeometry();
	void BuildPSOs();
	void BuildFrameResources();
	void BuildMaterial(int& index, int texIndex, const std::string& name, float roughness = 0.5f, const DirectX::XMFLOAT4& diffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f }, const DirectX::XMFLOAT3& fresnel = { 0.05f, 0.05f, 0.05f });
	void BuildMaterials();
	void BuildRenderItems();
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);


	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

private:

	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	FrameResource* mCurrFrameResource = nullptr;
	int mCurrFrameResourceIndex = 0;

	UINT mCbvSrvDescriptorSize = 0;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
	std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;
	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mShaders;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> mPSOs;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	// List of all the render items.
	std::unordered_map<std::string, std::unique_ptr<RenderItem>> mAllRitems;	//Makes it easier to know what you're accessing (i.e "Tiles" rather than 0)

	// Render items divided by PSO.
	std::vector<RenderItem*> mOpaqueRitems;

	UINT mInstanceCount = 0;

	bool mFrustumCullingEnabled = true;

	DirectX::BoundingFrustum mCamFrustum;

	PassConstants mMainPassCB;

	Camera mCamera;

	POINT mLastMousePos;

	//Audio
	GameAudio mGameAudio;
	DeltaTimer mAudioTimer; //Changing track after a while

	//Combat
	CombatController mCombatController;
	Player mPlayer;

	//States
	StateManager mStateManager;

	///Will be moved into its own class, just left here for ease of testing !!!Move in future
	int enemyHealth = 100;

	//Viewport Sprites
	SpriteManager mSprites;
	//std::unique_ptr<DX::TextConsole> mConsole;

};