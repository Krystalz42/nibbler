#pragma once

#include <cores/Univers.hpp>
#include <KINU/SystemsManager.hpp>

class CollisionSystem : public KINU::System {
public:
	CollisionSystem(Univers &univers);
	CollisionSystem() = delete;
	~CollisionSystem() = default;
	CollisionSystem &operator=(const CollisionSystem &) = delete;
	CollisionSystem(const CollisionSystem &) = delete;



	virtual void update();

private:
	void cleanVector(std::vector<KINU::Entity> &vec);
	Univers &univers_;
	void createAppleBySnake(KINU::Entity);
	void checkCollision(KINU::Entity entityHead, KINU::Entity entityCheck);
	std::set<KINU::Entity> entitiesToKill;
};



