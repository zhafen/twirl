#include <memory>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>
#include "systems.h"

void RenderSystem::addEntity(EntityID entityID, const RenderComponent& render_comp) {
    render_components[entityID] = render_comp;
}

void RenderSystem::update(float dt) {
    // Logic to update render components if needed
}

void RenderSystem::render(sf::RenderWindow& window) {
    for (auto& [id, render_comp] : render_components) {
        window.draw(render_comp.shape);
    }
}

ECS::ECS() : nextEntityID(0) {}

EntityID ECS::createEntity() {
    EntityID newID = nextEntityID++;
    entities.emplace_back(Entity{newID});
    return newID;
}

void ECS::addSystem(std::unique_ptr<System> system) {
    systems.push_back(std::move(system));
}

void ECS::updateSystems(float dt) {
    for (auto& system : systems) {
        system->update(dt);
    }
}