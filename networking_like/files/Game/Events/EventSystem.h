#pragma once
#include "Utils/Imports/common.h"

enum class GameEvent {
	SERVER__CLIENT_CONNECTED,
	SERVER__CLIENT_DISCONNECTED,
	SERVER__STARTED,
	SERVER__STOPPED,
	
	CLIENT__CONNECTED,
	CLIENT__DISCONNECTED,
	CLIENT__STARTED,
	CLIENT__STOPPED,
	CLIENT__PEER_ADDED,
	CLIENT__PEER_REMOVED,

};

class EventData {
public:
	GameEvent event;
	virtual ~EventData() = default;
};

using EventCallback = std::function<void(const EventData*)>;
using CallbackID = int;

class EventSystem {
public:
	static CallbackID register_event(GameEvent event, EventCallback callback);
	static void unregister_event(GameEvent event, CallbackID callbackID);
	static void trigger_event(GameEvent event, const EventData* data = nullptr);

private:
	static std::unordered_map<GameEvent, std::unordered_map<CallbackID, EventCallback>> callbacks;
	static CallbackID next_callback_id;
};