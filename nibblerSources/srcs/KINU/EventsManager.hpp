#pragma once

#include "Pool.hpp"
#include <unordered_map>
#include <vector>
#include <memory>
#include <typeindex>
#include <cstdint>
#include <iostream>

namespace KINU {

	class World;

	struct BaseEvent {
		using Id = uint8_t;
	protected:
		static Id nextId;
	};

	template<typename T>
	struct Event : BaseEvent {
		// Returns the unique id of Event<T>
		static Id getId() {
			static auto id = nextId++;
			return id;
		}
	};

	class EventsManager {
	public:
		EventsManager() = default;
		~EventsManager() = default;
		EventsManager &operator=(const EventsManager &) = delete;
		EventsManager(const EventsManager &) = delete;

		template<typename T>
		void emitEvent(T event);

		template<typename T, typename ... Args>
		void emitEvent(Args &&... args);

		template<typename T>
		std::vector<T> getEvents();

		void destroyEvents();

		template<typename T>
		void destroy();

	private:
		template<typename T>
		std::shared_ptr<Pool<T>> accommodateEvent();
		std::mutex mutex_;

		std::unordered_map<std::type_index, std::shared_ptr<AbstractPool>> eventPools;
	};

	template<typename T>
	void EventsManager::emitEvent(T event) {
		std::lock_guard<std::mutex> guard(mutex_);
		std::shared_ptr<Pool<T>> eventPool = accommodateEvent<T>();
		eventPool->add(event);
	}

	template<typename T, typename ... Args>
	void EventsManager::emitEvent(Args &&... args) {
		T event(std::forward<Args>(args) ...);
		emitEvent<T>(event);
	}

	template<typename T>
	std::shared_ptr<Pool<T>> EventsManager::accommodateEvent() {
		if (eventPools.find(std::type_index(typeid(T))) == eventPools.end()) {
			std::shared_ptr<Pool<T>> pool(new Pool<T>(0));
			eventPools.insert(std::make_pair(std::type_index(typeid(T)), pool));
		}
		return std::static_pointer_cast<Pool<T>>(eventPools.at(std::type_index(typeid(T))));
	}

	template<typename T>
	std::vector<T> EventsManager::getEvents() {
		std::lock_guard<std::mutex> guard(mutex_);
		return accommodateEvent<T>()->getData();
	}

	template<typename T>
	void EventsManager::destroy() {
		std::lock_guard<std::mutex> guard(mutex_);
		std::shared_ptr<Pool<T>> eventPool = accommodateEvent<T>();
		eventPool->clear();
	}
}
