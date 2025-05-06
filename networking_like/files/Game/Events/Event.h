#pragma once
#include "Utils/Imports/common.h"

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

	static void unregister_callback(CallbackID callback_id) {
		auto it = callbacks.find(callback_id);
		if (it != callbacks.end()) {
			callbacks.erase(it);
		}
	}

	static void trigger(const EventData& data) {
		for (const auto& [id, callback] : callbacks) {
			callback(data);
		}
	}

	static CallbackID next_callback_id;
	static std::unordered_map<CallbackID, Callback> callbacks;
};


// Set defaults
template <typename EventData>
typename Event<EventData>::CallbackID Event<EventData>::next_callback_id = 0;

template <typename EventData>
std::unordered_map<typename Event<EventData>::CallbackID, typename Event<EventData>::Callback> Event<EventData>::callbacks;