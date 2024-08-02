#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <SDL2/SDL_rect.h>
#include <cstring>
#include <enet/enet.h>

#include "Vector.hpp"

#define MESSAGE_VERSION 1
#define MAX_N_PLAYER 4
#define MAX_N_MOB 25
#define MSGMAP_DATALEN 512

enum mobType { BASIC };

enum MessageType {
  PLR_CO,
  PLR_DISCO,
  PLR_UPDATE,
  PLR_ID,
  MOB_UPDATE,
  GAME_STATE,
  MAP
};

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
  float angle;
  Vector pos;
  Vector vel;
};

struct MessagePlayerID {
  int id;
};

struct MessageMobUpdate {
  int id;
  mobType type;
  float angle;
  Vector pos;
  Vector vel;
};

struct MessageGameState {
  int nplayer;
  MessagePlayerUpdate players[MAX_N_PLAYER];
  int nmob;
  MessageMobUpdate mob[MAX_N_MOB];
};

struct MessageMap {
  int idPack;
  int nPack;
  int width;
  int tileSize;
  int dataLen;
  int data[MSGMAP_DATALEN];
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
