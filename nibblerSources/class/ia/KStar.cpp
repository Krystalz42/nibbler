#include "KStar.hpp"
#include <iostream>
#include <cmath>
#include <iomanip>
#include <cstdlib>
#include <type_traits>
#include <array>

/** Constant **/

std::array<KStar::Vec2, 8> const KStar::directions = { {
	Vec2(0,  1),
	Vec2(1,  0),
	Vec2(0,  -1),
	Vec2(-1, 0),
	Vec2(-1, -1),
	Vec2(1,  1),
	Vec2(-1, 1),
	Vec2(1,  -1)
}};

/** Node **/

KStar::Node::operator bool() const {
	return	H != -1;
}

KStar::Node::Node(const KStar::Vec2 vec, double H, double G, double F, Vec2 parent)
	: vec(vec), H(H), G(G), F(F), parent(parent) {

}

bool KStar::Node::operator==(const KStar::Node &rhs) const {
	return vec == rhs.vec &&
		   H == rhs.H &&
		   G == rhs.G &&
		   F == rhs.F &&
		   parent == rhs.parent;
}

bool KStar::Node::operator!=(const KStar::Node &rhs) const {
	return !(rhs == *this);
}

std::ostream &operator<<(std::ostream &os, const KStar::Node &node) {
	os << "vec: " << node.vec << " H: " << node.H << " G: " << node.G << " F: "<< node.F << " parent: " << node.parent;
	return os;
}

KStar::Node::Node(const KStar::Node &rhs) {
	*this = rhs;
}

KStar::Node &KStar::Node::operator=(const KStar::Node &rhs) {
	vec = rhs.vec;
	H = rhs.H;
	G = rhs.G;
	F = rhs.F;
	parent = rhs.parent;
	return *this;
}

void KStar::Node::clear() {
	vec.x = 0;
	vec.y = 0;
	H = -1;
	G = -1;
	F = -1;
	parent.x = 0;
	parent.y = 0;
}

/** Vec2 **/

KStar::Vec2::Vec2(int x, int y) : x(x), y(y) {

}

KStar::Vec2 KStar::Vec2::operator+(const KStar::Vec2 &lhs) {
	return KStar::Vec2(x + lhs.x, y + lhs.y);
}

KStar::Vec2::Vec2(const KStar::Vec2 &vec) {
	*this = vec;
}

bool KStar::Vec2::operator==(const KStar::Vec2 &rhs) const {
	return x == rhs.x &&
		   y == rhs.y;
}

bool KStar::Vec2::operator!=(const KStar::Vec2 &rhs) const {
	return !(rhs == *this);
}

std::ostream &operator<<(std::ostream &os, const KStar::Vec2 &vec2) {
	os << "x: " << vec2.x << " y: " << vec2.y;
	return os;
}

KStar::Vec2 &KStar::Vec2::operator=(const KStar::Vec2 &rhs) {
	x = rhs.x;
	y = rhs.y;
	return *this;
}

/** KStar **/

KStar::KStar() :
		direction(8),
		closeMap_(0),
		openMap_(0),
		collision_(0) {

}

void KStar::addCollision(KStar::Vec2 pos) {
	assert(!isOverflow(pos));
	collision_(pos.x, pos.y) = true;
}

void KStar::removeCollision(KStar::Vec2 pos) {
	assert(!isOverflow(pos));
	collision_(pos.x, pos.y) = false;
}

void KStar::clearCollisions() {
	collision_.fill(false);
}

void KStar::setHeuristic(HeuristicFunction function) {
	heuristic_ = std::bind(function, std::placeholders::_1, std::placeholders::_2);
}

void KStar::setDiagonalMovement(bool enable_) {
	direction = enable_ ? 8 : 4;
}

KStar::Path KStar::searchPath(KStar::Vec2 source, KStar::Vec2 target) {

	Path path;

	openMap_.fill(Node());
	closeMap_.fill(Node());
	if (isCollision(target)) {
		return path;
	}
	openMap_(source.x, source.y) = Node(source,
			heuristic_(source, target),
			heuristic_(source, source),
			heuristic_(source, target) + heuristic_(source, source),
			Vec2());

	for (; !openMap_.empty() ;) {
		Node currentNode = getLowestNode();
		if (!currentNode)
			return path;
		if (currentNode.vec == target) {
			closeMap_(currentNode.vec.x, currentNode.vec.y) = currentNode;
			break;
		}

		closeMap_(currentNode.vec.x, currentNode.vec.y) = currentNode;
		openMap_(currentNode.vec.x, currentNode.vec.y).clear();
		for (int idxDir = 0; idxDir < direction; ++idxDir) {
			Vec2 currentVec2 = currentNode.vec + directions[idxDir];
				if (!isOverflow(currentVec2) && !isCollision(currentVec2) && !closeMap_(currentVec2.x, currentVec2.y)) {

				double tempH = heuristic_(currentVec2, target);
				double tempG = heuristic_(currentVec2, source);

				Node newNode = Node(currentVec2, tempH, tempG, tempG + tempH, currentNode.vec);
				if (!openMap_(currentVec2.x, currentVec2.y)) {
					openMap_(currentVec2.x, currentVec2.y) = newNode;
				} else {
					if (tempG < openMap_(currentVec2.x, currentVec2.y)) {
						openMap_(currentVec2.x, currentVec2.y) = newNode;
					}
				}
			}
		}
	}
	Node traceNode = closeMap_(target.x, target.y);
	path.push_back(traceNode.vec);
	for (; traceNode.vec != source ;) {
		traceNode = closeMap_(traceNode.parent.x, traceNode.parent.y);
		path.push_back(traceNode.vec);
	}
	std::reverse(path.begin(), path.end());
//	print(source, target, path);
	return path;
}

bool KStar::isOverflow(KStar::Vec2 v) {
	return !(v.x >= 0 && v.x < worldSize_.x && v.y >= 0 && v.y < worldSize_.y);
}

void KStar::setWorldSize(KStar::Vec2 worldSize) {
	worldSize_ = worldSize;

	openMap_ = MutantGrid<KStar::Node>(worldSize.x, worldSize.y);
	closeMap_ = MutantGrid<KStar::Node>(worldSize.x, worldSize.y);
	collision_ = MutantGrid<bool>(worldSize.x, worldSize.y);
}


KStar::Node KStar::getLowestNode() {
	return *std::min_element(openMap_.begin(), openMap_.end(),
			[](Node const &lNode, Node const &rNode){
				if (!rNode)
					return true;
				if (!lNode)
					return false;
				return lNode.F < rNode.F;
	});
}

bool KStar::isCollision(KStar::Vec2 vec) {
	assert(!isOverflow(vec));
	return collision_(vec.x, vec.y);
}


int KStar::getDirection() const {
	return direction;
}

const std::array<KStar::Vec2, 8> &KStar::getDirections() {
	return directions;
}


/**  HEURISTIC **/

KStar::Vec2 KStar::Heuristic::getDelta(KStar::Vec2, KStar::Vec2) {
	return Vec2();
}

double KStar::Heuristic::manhattan(KStar::Vec2 source, KStar::Vec2 target) {
	auto delta = std::move(KStar::Heuristic::getDelta(source, target));
	return static_cast<uint>(10 * (delta.x + delta.y));
}

double KStar::Heuristic::euclidean(KStar::Vec2 source, KStar::Vec2 target) {
	return std::sqrt(std::pow(source.x - target.x, 2) + std::pow(source.y - target.y, 2));
}
