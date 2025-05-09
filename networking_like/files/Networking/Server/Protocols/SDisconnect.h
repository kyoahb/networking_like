#pragma once
#include "SProtocol.h"
#include "Networking/Shared/FutureResults/DisconnectResult.h"
#include "Networking/Shared/NetPeer.h"
#include "Utils/General/TimeUtils.h"
/*
 * @brief PendingDisconnect class
 *
 * Represents a pending disconnect request operation sent by the server.
 */
class PendingDisconnect {
public:
	DisconnectResultReason reason = DisconnectResultReason::UNKNOWN; // Reason for the disconnect result
	DisconnectResultType type = DisconnectResultType::UNKNOWN; // Type of disconnect result
	uint64_t request_time = 0; // Time when the disconnect request was made
};

/*
 * @brief SDisconnect class
 *
 * Represents the disconnect protocol for the server.
 * Handles disconnect requests and manages pending disconnects.
 */
class SDisconnect : public SProtocol {
public:
	SDisconnect(std::shared_ptr<Server> _server);
	~SDisconnect();

	void start() override;
	void stop() override;
	void update() override;
	void destroy() override;
	void packet_event(const ENetEvent& event, std::optional<NetPeer> peer) override;

	// Called from server. Usually this wont happen, but this is a built-in protocol
	DisconnectResult disconnect_peer(ENetPeer* peer, DisconnectResultReason reason = DisconnectResultReason::SERVER_REQUESTED);

private:
	std::unordered_map<ENetPeer*, PendingDisconnect> pending_disconnects; // List of pending disconnects

	const int DISCONNECT_TIMEOUT = 5000; // Timeout limit in milliseconds
	const int CHECK_INTERVAL = 1000; // Check interval in milliseconds
	uint64_t last_check_time = 0; // Last time work_pending_disconnects() was called

	void add_pending_disconnect(ENetPeer* peer, DisconnectResultReason reason = DisconnectResultReason::SERVER_REQUESTED); // Add a pending disconnect
	void work_pending_disconnects();
};