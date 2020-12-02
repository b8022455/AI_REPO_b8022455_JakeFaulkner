#include "Enemy.h"

void Enemy::InitEnemyPosition(int instance, DirectX::XMFLOAT3 position, int matIndex, std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems)
{
	mAllRitems["Enemy"]->Instances.at(instance).MaterialIndex = matIndex;
	
	mAllRitems["Enemy"]->Instances.at(instance).World._41 = position.x;
	mAllRitems["Enemy"]->Instances.at(instance).World._42 = position.y;
	mAllRitems["Enemy"]->Instances.at(instance).World._43 = position.z;
}

void Enemy::UpdatePosition(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems)
{
	position = DirectX::XMFLOAT3(mAllRitems["Enemy"]->Instances.at(0).World._41, mAllRitems["Enemy"]->Instances.at(0).World._42,
		mAllRitems["Enemy"]->Instances.at(0).World._43);
}

void Enemy::SetPosition(DirectX::XMFLOAT3 newPosition, std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems)
{
	position = newPosition;		//Updates the position locally first

	//Updates position on the object
	mAllRitems["Enemy"]->Instances.at(0).World._41 = newPosition.x;
	mAllRitems["Enemy"]->Instances.at(0).World._42 = newPosition.y;
	mAllRitems["Enemy"]->Instances.at(0).World._43 = newPosition.z;
}

DirectX::XMFLOAT3 Enemy::GetPosition()
{
	return position;
}

void Enemy::AddAnotherEnemy(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems)		///Could add more parameters for pos or material of new instance!!!
{
	//Creates new instance of the enemy
	InstanceData id;
	id.World = MathHelper::Identity4x4();
	id.MaterialIndex = 2;
	mAllRitems["Enemy"]->Instances.push_back(id);
}

