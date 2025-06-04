#include "pch.h"
#include "TransformSystem.h"

void TransformSystem::Update(ECSManager& ecs, float dt)
{
	worldMatrices.clear();
	ComponentMask mask = (1ULL << TransformComponent::StaticTypeID);
	ecs.ForEach(mask, [&](Entity entity)
		{
			TransformComponent* t = ecs.GetComponent<TransformComponent>(entity);
			if (!t) return;
			// Recalcule la matrice monde (avec hierarchie) et la stocke
			XMMATRIX wm = ComputeWorldMatrix(entity, ecs);
			worldMatrices[entity.id] = wm;
		});
}

XMMATRIX TransformSystem::ComputeWorldMatrix(Entity entity, ECSManager& ecs)
{
	TransformComponent* t = ecs.GetComponent<TransformComponent>(entity);
	assert(t && "Entity must have a TransformComponent");

	// Si ce transform n’est pas dirty, et que la matrice existe deja, on la renvoie
	if (!t->dirty)
	{
		auto it = worldMatrices.find(entity.id);
		if (it != worldMatrices.end())
			return it->second;
	}

	// 1) Calcul de la matrice locale (Scale×Rotation×Translation)
	XMVECTOR pos = XMLoadFloat3(&t->position);
	XMVECTOR rot = XMLoadFloat4(&t->rotation);
	XMVECTOR scl = XMLoadFloat3(&t->scale);

	XMMATRIX mRot = XMMatrixRotationQuaternion(rot);
	XMMATRIX mScl = XMMatrixScalingFromVector(scl);
	XMMATRIX mTrans = XMMatrixTranslationFromVector(pos);
	XMMATRIX local = mScl * mRot * mTrans;

	XMMATRIX world = local;

	// 2) Si l’entite a un parent, on recupere sa world matrix
	if (t->parent.id != UINT32_MAX)
	{
		TransformComponent* tp = ecs.GetComponent<TransformComponent>(t->parent);
		if (tp)
		{
			XMMATRIX parentWorld = ComputeWorldMatrix(t->parent, ecs);
			// Convention : on applique d’abord le local puis lie au parent
			
			//world = local * parentWorld; // Lui ou l'autre ?
			world = parentWorld * local;
		}
	}

	// 3) On marque comme “non dirty” et on renvoie
	t->dirty = false;
	return world;
}