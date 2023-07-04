#pragma once

#include "Core/TypeDef.h"

namespace soft
{
	class Scene2D;
}

using namespace soft;

void AddStaticObjects(Scene2D* scene, byte* mapValues, size_t width, size_t height,
	byte* blockCells, byte blockCellCount);

void AddPlayer(Scene2D* scene, ID id, const Vec2& pos, size_t width, size_t height);

void AddMapMonsters(Scene2D* scene, const byte* mapMonsterIds, size_t width, size_t height);

void AddMapRenderer(Scene2D* scene, const byte* mapValues, size_t width, size_t height);

void AddUINode(Scene2D* scene);