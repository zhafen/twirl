#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <typeindex>

using EntityID = std::size_t;
using ComponentID = std::type_index;

class Entity {
   public:
    EntityID id;
    // Additional entity-related methods and data
};

class Component {
   public:
    virtual ~Component() = default;
};

class RenderComponent : public Component {
   public:
    // Additional render component-related methods and data
    sf::CircleShape shape;
};

class System {
   public:
    virtual void update(float dt) = 0;
    // Additional system-related methods and data
};

class RenderSystem : public System {
   public:
    void addEntity(EntityID entityID, const RenderComponent& render_comp);

    void update(float dt) override;

    void render(sf::RenderWindow& window);

   private:
    std::unordered_map<EntityID, RenderComponent> render_components;
};

class ECS {
   public:
    EntityID createEntity();

    template <typename T, typename... Args>
    void addComponent(EntityID entityID, Args&&... args) {
        ComponentID compID = std::type_index(typeid(T));
        components[entityID][compID] = std::make_unique<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    T* getComponent(EntityID entityID) {
        ComponentID compID = std::type_index(typeid(T));
        auto it = components[entityID].find(compID);
        if (it != components[entityID].end()) {
            return static_cast<T*>(it->second.get());
        }
        return nullptr;
    }

    void addSystem(std::unique_ptr<System> system);

    void updateSystems(float dt);

   private:
    EntityID nextEntityID;
    std::vector<Entity> entities;
    std::unordered_map<EntityID,
                       std::unordered_map<ComponentID, std::unique_ptr<Component>>>
        components;
    std::vector<std::unique_ptr<System>> systems;
};

#endif  // SYSTEMS_H