// #ifndef SCENE_H
// #define SCENE_H
// 
// #include <vector>
// #include <memory>
// #include "component.h"
// #include "system.h"
// 
// namespace cc {
// 
// class Scene {
// public:
//     Scene();
//     ~Scene();
// 
//     void addEntity(const Entity& entity);
//     void addComponent(const Entity& entity, std::shared_ptr<Component> component);
//     void addSystem(std::shared_ptr<System> system);
// 
//     void update(float deltaTime);
// 
// private:
//     std::vector<Entity> entities;
//     std::vector<std::shared_ptr<Component>> components;
//     std::vector<std::shared_ptr<System>> systems;
// };
// 
// } // namespace cc
// 
// #endif // SCENE_H