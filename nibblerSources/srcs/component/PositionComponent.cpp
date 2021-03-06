

#include <iostream>
#include "PositionComponent.hpp"


PositionComponent::PositionComponent(int x, int y) :  x(x), y(y) {
}

PositionComponent::PositionComponent(std::pair<size_t, size_t> pair) : x(
		pair.first), y(pair.second) {

}

bool PositionComponent::operator==(const PositionComponent &rhs) const {
	return y == rhs.y && x == rhs.x;
}

bool PositionComponent::operator!=(const PositionComponent &rhs) const {
	return !(rhs == *this);
}

PositionComponent::PositionComponent(
		PositionComponent const &positionComponent) {
	*this = positionComponent;
}

PositionComponent &
PositionComponent::operator=(PositionComponent const &motionComponent) {
	if (this != &motionComponent) {
		y = motionComponent.y;
		x = motionComponent.x;
	}
	return *this;
}

std::ostream &operator<<(std::ostream &os, const PositionComponent &component) {
	os << "y: " << component.y << " x: " << component.x;
	return os;
}

PositionComponent::operator bool() const {
	return x != -1 && y != -1;
}

