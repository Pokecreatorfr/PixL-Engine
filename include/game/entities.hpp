#pragma once

namespace game
{
    enum class EntityType
    {
        Unknown,
        Player,
        NPC,
        Item,
        Obstacle,
        SpawnPoint
    };

    struct Entity
    {
        EntityType type = EntityType::Unknown;
        float posX = 0.0f;
        float posY = 0.0f;
        float width = 1.0f;
        float height = 1.0f;
    };

    struct EntityPlayer : Entity
    {
        EntityType type = EntityType::Player;
    };

    struct EntityNPC : Entity
    {
        EntityType type = EntityType::NPC;
    };

    struct EntityItem : Entity
    {
        EntityType type = EntityType::Item;
    };

    struct EntityObstacle : Entity
    {
        EntityType type = EntityType::Obstacle;
    };

    struct EntitySpawnData : Entity
    {
        EntityType type = EntityType::SpawnPoint;
    };

}
