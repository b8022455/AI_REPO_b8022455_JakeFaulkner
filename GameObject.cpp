#include "GameObject.h"
#include "GameApp.h"

void GameObject::AddRenderItemInstance(const std::string & renderItemName)
{
	mpInstance = GameApp::Get().AddRenderItemInstance(renderItemName);
	assert(mpInstance);
}

void GameObject::Initialize(const std::string & renderItemName)
{
	AddRenderItemInstance(renderItemName);
}
