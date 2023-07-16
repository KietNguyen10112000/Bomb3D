#include "VictoryTowerGenerator.h"

#include "Components2D/Physics/RigidBody2D.h"
#include "Components2D/Rendering/SpriteRenderer.h"

#include "GameConfig.h"

#include "COLLISION_MASK.h"
#include "TAG.h"

#include "VictoryTowerScript.h"

VictoryTowerGenerator::VictoryTowerGenerator()
{
    m_collider = MakeShared<CircleCollider>(Vec2(0,0), 30.0f);
}

Handle<GameObject2D> VictoryTowerGenerator::New()
{
    auto wall = mheap::New<GameObject2D>(GameObject2D::DYNAMIC);
    wall->NewComponent<RigidBody2D>(RigidBody2D::STATIC, m_collider)->CollisionMask() = COLLISION_MASK::WALL;
    wall->NewComponent<VictoryTowerScript>();

    auto renderer = wall->NewComponent<SpriteRenderer>("buildings/victory_tower_0.png", AARect(), Transform2D());
    //renderer->Sprite().Transform().Scale() = Vec2(0.5);
    renderer->ClearAABB();
    renderer->Sprite().SetAnchorPoint({ 0.5f,0.75f });

    wall->Tag() = TAG::BUILDING_0;

    return wall;
}
