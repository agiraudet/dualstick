
// enet_server.cpp
#include <enet/enet.h>
#include <iostream>
#include <vector>

void broadcastMessage(ENetHost *server, ENetPacket *packet) {
  enet_host_broadcast(server, 0, packet);
}

int main(int argc, char **argv) {
  if (enet_initialize() != 0) {
    std::cerr << "An error occurred while initializing ENet.\n";
    return EXIT_FAILURE;
  }

  ENetAddress address;
  ENetHost *server;
  address.host = ENET_HOST_ANY;
  address.port = 1234;

  server = enet_host_create(&address, 32, 2, 0, 0);
  if (server == nullptr) {
    std::cerr
        << "An error occurred while trying to create an ENet server host.\n";
    exit(EXIT_FAILURE);
  }

  std::cout << "Server started...\n";

  while (true) {
    ENetEvent event;
    while (enet_host_service(server, &event, 1000) > 0) {
      switch (event.type) {
      case ENET_EVENT_TYPE_CONNECT:
        std::cout << "A new client connected from " << event.peer->address.host
                  << ":" << event.peer->address.port << "\n";
        break;

      case ENET_EVENT_TYPE_RECEIVE:
        std::cout << "A packet of length " << event.packet->dataLength
                  << " containing " << event.packet->data
                  << " was received from " << event.peer->data << " on channel "
                  << event.channelID << "\n";
        broadcastMessage(server, event.packet);
        enet_packet_destroy(event.packet);
        break;

      case ENET_EVENT_TYPE_DISCONNECT:
        std::cout << "Client disconnected.\n";
        break;

      default:
        break;
      }
    }
  }

  enet_host_destroy(server);
  enet_deinitialize();
  return EXIT_SUCCESS;
}
