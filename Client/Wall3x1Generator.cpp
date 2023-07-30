#include "Wall3x1Generator.h"

#include "Components2D/Physics/RigidBody2D.h"
#include "Components2D/Rendering/SpriteRenderer.h"

#include "Objects2D/Physics/Colliders/RectCollider.h"

#include "GameConfig.h"

#include "COLLISION_MASK.h"
#include "TAG.h"

#include "Wall3x1Script.h"

Wall3x1Generator::Wall3x1Generator()
{
    float w = 3 * GameConfig::CELL_SIZE;
    float h = GameConfig::CELL_SIZE;
    m_collider = MakeShared<RectCollider>(Rect(-w / 2.0f, -h / 2.0f, w, h));
}

Handle<GameObject2D> Wall3x1Generator::New()
{
    auto wall = mheap::New<GameObject2D>(GameObject2D::DYNAMIC);
    wall->NewComponent<RigidBody2D>(RigidBody2D::STATIC, m_collider)->CollisionMask() = COLLISION_MASK::WALL;

    wall->NewComponent<Wall3x1Script>();

    auto renderer = wall->NewComponent<SpriteRenderer>("buildings/wall3x1.png", AARect(), Transform2D());
    renderer->Sprite().FitTextureSize({ 3 * GameConfig::CELL_SIZE, GameConfig::CELL_SIZE });
    renderer->ClearAABB();
    renderer->Sprite().SetAnchorPoint({ 0.5f,0.5f });
    
    wall->Tag() = TAG::BUILDING_0;

    return wall;
}
