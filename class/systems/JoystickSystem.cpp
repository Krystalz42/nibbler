#include <component/JoystickComponent.hpp>
#include <component/MotionComponent.hpp>
#include "JoystickSystem.hpp"
#include <KNU/World.hpp>
#include <events/JoystickEvent.hpp>

JoystickSystem::JoystickSystem() {
	requireComponent<JoystickComponent>();
	requireComponent<MotionComponent>();
}

void JoystickSystem::update() {
	auto events = getWorld().getEventManager().getEvents<JoystickEvent>();
	log_success("JoystickSystem::update on %d events", events.size());
	for (auto &event : events) {
		auto entity = getWorld().getEntityManager().getEntityByTag(
				event.tag_player);

		if (entity.getId() != -1 && entity.hasComponent<JoystickComponent>()) {
			auto &joystickComponent = entity.getComponent<JoystickComponent>();
			joystickComponent.direction = event.direction;
		}
	}
	for (auto &entity : getEntities()) {
		auto &motionComponent = entity.getComponent<MotionComponent>();
		auto &joystickComponent = entity.getComponent<JoystickComponent>();
		log_error("Match Mot[%d] Joy[%d] MotHor[%d] JoyHor[%d] MotVer[%d] JoyVer[%d]",
				  motionComponent.direction,
					joystickComponent.direction,
				  motionComponent.direction & DIRECTION_HORIZONTAL,
				  joystickComponent.direction & DIRECTION_HORIZONTAL,
				  motionComponent.direction & DIRECTION_VERTICAL,
				  joystickComponent.direction & DIRECTION_VERTICAL
					);

		if ((motionComponent.direction & DIRECTION_HORIZONTAL &&
				!(joystickComponent.direction & DIRECTION_HORIZONTAL)) ||
			(motionComponent.direction & DIRECTION_VERTICAL &&
					!(joystickComponent.direction & DIRECTION_VERTICAL))) {
			log_error("MatchDiretion!");
			motionComponent.direction = joystickComponent.direction;
		}
	}
}

JoystickSystem::~JoystickSystem() {

}
