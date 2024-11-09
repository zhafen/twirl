// #include "scene.h"
// 
// Scene::Scene() {
//     // Constructor implementation
// }
// 
// Scene::~Scene() {
//     // Destructor implementation
// }
// 
// void Scene::addEntity(const Entity& entity) {
//     entities.push_back(entity);
// }
// 
// void Scene::addComponent(const Entity& entity, std::shared_ptr<Component> component) {
//     // Assuming each entity has a unique ID and components are associated with entities
//     components.push_back(component);
// }
// 
// void Scene::addSystem(std::shared_ptr<System> system) {
//     systems.push_back(system);
// }
// 
// void Scene::update(float deltaTime) {
//     for (auto& system : systems) {
//         system->update(entities, components, deltaTime);
//     }
// }