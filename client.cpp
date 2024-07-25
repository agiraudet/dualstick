
// enet_client.cpp
#include <enet/enet.h>
#include <iostream>
#include <thread>

void receiveMessages(ENetHost *client) {
  while (true) {
    ENetEvent event;
    while (enet_host_service(client, &event, 1000) > 0) {
      switch (event.type) {
      case ENET_EVENT_TYPE_RECEIVE:
        std::cout << "Message received: " << event.packet->data << "\n";
        enet_packet_destroy(event.packet);
        break;
      default:
        break;
      }
    }
  }
}

int main(int argc, char **argv) {
  if (enet_initialize() != 0) {
    std::cerr << "An error occurred while initializing ENet.\n";
    return EXIT_FAILURE;
  }

  ENetHost *client = enet_host_create(nullptr, 1, 2, 0, 0);
  if (client == nullptr) {
    std::cerr
        << "An error occurred while trying to create an ENet client host.\n";
    exit(EXIT_FAILURE);
  }

  ENetAddress address;
  ENetPeer *peer;
  enet_address_set_host(&address, "localhost");
  address.port = 1234;

  peer = enet_host_connect(client, &address, 2, 0);
  if (peer == nullptr) {
    std::cerr << "No available peers for initiating an ENet connection.\n";
    exit(EXIT_FAILURE);
  }

  std::thread receiveThread(receiveMessages, client);
  receiveThread.detach();

  std::cout << "Connected to server. Type messages to send:\n";

  std::string message;
  while (true) {
    std::getline(std::cin, message);
    if (message == "exit")
      break;

    ENetPacket *packet = enet_packet_create(
        message.c_str(), message.length() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
    enet_host_flush(client);
  }

  enet_peer_disconnect(peer, 0);
  enet_host_destroy(client);
  enet_deinitialize();
  return EXIT_SUCCESS;
}
