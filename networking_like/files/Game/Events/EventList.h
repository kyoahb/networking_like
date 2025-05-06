#pragma once
#include "Event.h"

#include "Networking/Packets/Data/ClientConnectBegin.h"
#include "Networking/Shared/NetPeer.h"

namespace Events {
	namespace Server {
		// Server started
		struct StartData : public BaseEventData {
			explicit StartData() {};
		};
		using Start = Event<StartData>;

		// Server stopped
		struct StopData : public BaseEventData {
			explicit StopData() {};
		};
		using Stop = Event<StopData>;

		// Initial client connection (Received event with type CONNECT)
		struct ClientConnectData : public BaseEventData {
			ENetPeer* peer;
			explicit ClientConnectData(ENetPeer* peer) : peer(peer) {}
		};
		using ClientConnect = Event<ClientConnectData>;

		// Client connection begin (Received event with type CLIENT_CONNECT_BEGIN)
		struct ClientBeginData : public BaseEventData {
			ENetPeer* peer;
			const ClientConnectBegin& client_connect_begin;
			explicit ClientBeginData(ENetPeer* peer, const ClientConnectBegin& client_connect_begin) : peer(peer), client_connect_begin(client_connect_begin) {}
		};
		using ClientBegin = Event<ClientBeginData>;

		// Client disconnected (Received event with type DISCONNECT)
		// Currently provides no disconnect reason: this is because there is no system in place 
		// (like there is with connect) to provide a reason with, if the client decides to disconnect themselves.
		struct ClientDisconnectData : public BaseEventData {
			ENetPeer* peer;
			std::optional<NetPeer> net_peer; // No value if client disconnected before being registered to a netpeer
			const std::string& message;
			explicit ClientDisconnectData(ENetPeer* peer, std::optional<NetPeer> net_peer, const std::string& message) : peer(peer), net_peer(net_peer), message(message) {}
		};
		using ClientDisconnect = Event<ClientDisconnectData>;
	}

	namespace Client {
		// Client started
		struct StartData : public BaseEventData {
			explicit StartData() {};
		};
		using Start = Event<StartData>;

		// Client stopped
		struct StopData : public BaseEventData {
			explicit StopData() {};
		};
		using Stop = Event<StopData>;

		// Client connected
		struct ConnectData : public BaseEventData {
			ENetPeer* peer;
			const std::string& address;
			int port;
			explicit ConnectData(ENetPeer* peer, const std::string& address, int port) : peer(peer), address(address), port(port) {};
		};
		using Connect = Event<ConnectData>;

		// Client disconnected
		struct DisconnectData : public BaseEventData {
			const std::string& message;
			explicit DisconnectData(const std::string& message) : message(message) {};
		};
		using Disconnect = Event<DisconnectData>;

		// Peer added (CLIENT_CONNECT_RELAY received)
		struct PeerAddedData : public BaseEventData {
			LocalNetPeer peer;
			explicit PeerAddedData(LocalNetPeer peer) : peer(peer) {};
		};
		using PeerAdded = Event<PeerAddedData>;

		// Peer removed (CLIENT_CONNECT_RELAY received)
		struct PeerRemovedData : public BaseEventData {
			LocalNetPeer peer;
			explicit PeerRemovedData(LocalNetPeer peer) : peer(peer) {};
		};
		using PeerRemoved = Event<PeerRemovedData>;
	}
} // namespace Events
