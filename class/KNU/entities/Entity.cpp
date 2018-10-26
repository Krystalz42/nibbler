#include "Entity.hpp"
#include <KNU/utils/Signature.hpp>
#include <KNU/World.hpp>

namespace KNU {


	const Signature &Entity::getMask() {
		return mask;
	}

	void Entity::kill() {
		getEntitiesManager().killEntity(*this);
	}

	bool Entity::isAlive() const {
		return getEntitiesManager().isEntityAlive(*this);
	}

	EntitiesManager &Entity::getEntitiesManager() const {
		return *entityManager;
	}

	void Entity::tag(std::string tag) {
		getEntitiesManager().tagEntity(*this, tag);
	}

	bool Entity::hasTag(std::string tag) const {
		return getEntitiesManager().hasTaggedEntity(tag);
	}

	void Entity::group(std::string group) {
		getEntitiesManager().groupEntity(*this, group);
	}

	bool Entity::hasGroup(std::string group) const {
		return getEntitiesManager().hasEntityInGroup(*this, group);
	}
	//		unsigned int size;
	//		unsigned int capacity;
	//		World &world;
	//		ComponentManager &_componentManager;
	//		std::vector<Entity> _entitiesMap;
	//		std::unordered_map<std::string, Entity> _taggedEntity;
	//		std::unordered_map<std::string, std::set<Entity>> _groupedEntities;

	EntitiesManager::EntitiesManager(World &world,
									 ComponentManager &componentManager)
			: size(0),
			  capacity(BASE_ENTITIES_CAPACITY),
			  world(world),
			  _componentManager(componentManager) {
		_entitiesMap.resize(capacity);
	}


/** Entities Managements **/


	Entity &
	EntitiesManager::createEntity() {
		if (size == capacity) {
			capacity += BASE_ENTITIES_CAPACITY;
			_entitiesMap.resize(capacity);
		}
		unsigned int instance_entity = size;
		size++;
		_entitiesMap[instance_entity] = Entity(instance_entity);
		_entitiesMap[instance_entity].entityManager = this;
		return _entitiesMap[instance_entity];
	}

	void EntitiesManager::destroyEntity(Entity &entity) {
		assert(entity.id < static_cast<int>(size));
		assert(entity == _entitiesMap[entity.id]);
		size--;
		_entitiesMap[entity.id] = _entitiesMap[size];

	}

	void EntitiesManager::killEntity(Entity &entity) {
		world.destroyEntity(entity);
		entity.alive = false;
	}

/** Tag/Group Management **/
	bool EntitiesManager::hasTaggedEntity(std::string &tag) {
		return _taggedEntity.find(tag) != _taggedEntity.end();
	}

	void EntitiesManager::tagEntity(Entity &entity, std::string &tag) {
		_taggedEntity.insert(std::make_pair(tag, entity));
	}

	void EntitiesManager::groupEntity(Entity &entity, std::string &group) {
		if (!hasGroup(group)) {
			_groupedEntities.emplace(std::make_pair(group, std::set<Entity>()));
		}
		_groupedEntities[group].emplace(entity);
	}


	std::vector<Entity>
	EntitiesManager::getEntitiesByGroup(std::string const &group) {
		if (!hasGroup(group))
			return std::vector<Entity>();
		auto &set = _groupedEntities[group];
		return std::vector<Entity>(set.begin(), set.end());
	}

	unsigned long EntitiesManager::getGroupSize(std::string group) {
		return _groupedEntities[group].size();
	}

	bool EntitiesManager::hasTag(std::string const &tag) {
		return _taggedEntity.find(tag) != _taggedEntity.end();
	}

	bool EntitiesManager::hasGroup(std::string const &group) {
		return _groupedEntities.find(group) != _groupedEntities.end();
	}

	bool EntitiesManager::hasEntityInGroup(Entity const &entity, std::string group) {
		auto it = _groupedEntities.find(group);
		if (it != _groupedEntities.end()) {
			if (it->second.find(entity.id) != it->second.end()) {
				return true;
			}
		}
		return false;
	}

	bool EntitiesManager::isEntityAlive(Entity const &entity) const {
		return entity.alive;
	}

	Entity EntitiesManager::getEntityByTag(std::string const &tag) {
		assert(hasTag(tag));
		return _taggedEntity[tag];
	}
}