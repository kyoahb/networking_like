#pragma once
#include "Utils/Imports/common.h"
#include <future>
class BaseEventData {
public:
	virtual ~BaseEventData() = default;
};

template <typename EventData>
class Event {
	static_assert(std::is_base_of<BaseEventData, EventData>::value, "EventData must derive from BaseEventData");
public:
	using Callback = std::function<void(const EventData&)>;
	using CallbackID = int;

	static CallbackID register_callback(Callback callback) {
		CallbackID callback_id = next_callback_id++;
		callbacks[callback_id] = callback;
		return callback_id;
	}

	static bool is_callback_registered(CallbackID callback_id) {
		return callbacks.find(callback_id) != callbacks.end();
	}

	static void unregister_callback(CallbackID callback_id) {
		auto it = callbacks.find(callback_id);
		if (it != callbacks.end()) {
			callbacks.erase(it);
		}
	}

	static void trigger(const EventData& data) {
		LOG_SCOPE_TEST
		std::vector<std::future<void>> futures;

		for (auto it = callbacks.begin(); it != callbacks.end(); ) {
			if (it->second) { // Check if the callback is valid
				// Launch the callback asynchronously
				futures.push_back(std::async(std::launch::async, it->second, std::cref(data)));
				++it;
			}
			else {
				// Remove invalid callbacks
				Log::trace("Invalid callback for event type " + std::string(typeid(EventData).name()) + " removed. Callback ID: " + std::to_string(it->first));
				it = callbacks.erase(it);
			}
		}

		// Optionally, wait for all asynchronous tasks to complete
		/*for (auto& future : futures) {
			future.wait();
		}*/
	}

	static CallbackID next_callback_id;
	static std::unordered_map<CallbackID, Callback> callbacks;
};


// Set defaults
template <typename EventData>
typename Event<EventData>::CallbackID Event<EventData>::next_callback_id = 0;

template <typename EventData>
std::unordered_map<typename Event<EventData>::CallbackID, typename Event<EventData>::Callback> Event<EventData>::callbacks;