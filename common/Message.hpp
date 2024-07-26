#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <SDL2/SDL_rect.h>
#include <cstring>
#include <enet/enet.h>

#define MESSAGE_VERSION 1

enum MessageType { PLR_CO, PLR_DISCO, PLR_UPDATE };

struct MessageHeader {
  MessageType type;
  int version;
};

struct MessagePlayerCo {
  int id;
};

struct MessagePlayerDisco {
  int id;
};

struct MessagePlayerUpdate {
  int id;
  SDL_Rect rect;
};

template <typename T>
void serializeMessage(const T &message, char *data, MessageType type) {
  MessageHeader header = {type, MESSAGE_VERSION};
  std::memcpy(data, &header, sizeof(MessageHeader));
  std::memcpy(data + sizeof(MessageHeader), &message, sizeof(T));
}

template <typename T> T deserializeMessage(const char *data) {
  T message;
  std::memcpy(&message, data + sizeof(MessageHeader), sizeof(T));
  return message;
}

template <typename T>
ENetPacket *packageMessage(const T &message, MessageType type) {
  size_t dataLen = sizeof(MessageHeader) + sizeof(T);
  char *data = new char[dataLen];
  serializeMessage(message, data, type);
  ENetPacket *packet =
      enet_packet_create(data, dataLen, ENET_PACKET_FLAG_RELIABLE);
  delete[] data;
  return packet;
}

#endif
