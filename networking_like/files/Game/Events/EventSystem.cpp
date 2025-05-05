#include "EventSystem.h"

std::unordered_map<GameEvent, std::unordered_map<CallbackID, EventCallback>> EventSystem::callbacks;
CallbackID EventSystem::next_callback_id = 0;

CallbackID EventSystem::register_event(GameEvent event, EventCallback callback) {
	LOG_SCOPE_CLIENT;
	CallbackID id = next_callback_id++;
	callbacks[event][id] = callback;
	return id;
}

void EventSystem::unregister_event(GameEvent event, CallbackID callbackID) {
    auto eventIt = callbacks.find(event);
    if (eventIt != callbacks.end()) {
        eventIt->second.erase(callbackID);

        // If no callbacks left for this event, remove the event entry
        if (eventIt->second.empty()) {
            callbacks.erase(eventIt);
        }
    }
}

void EventSystem::trigger_event(GameEvent event, const EventData* data) {
    auto eventIt = callbacks.find(event);
    if (eventIt != callbacks.end()) {
        // Call all registered callbacks for this event
        for (const auto& [id, callback] : eventIt->second) {
            callback(data);
        }
    }
}