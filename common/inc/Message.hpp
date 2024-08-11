#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <SDL2/SDL_rect.h>
#include <array>
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
  PLR_DEAD,
  PLR_SHOOT,
  MOB_UPDATE,
  MOB_HIT,
  MOB_ATTACK,
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

struct MessagePlayerID {
  int id;
};

struct MessagePlayerDead {
  int id;
};

struct MessagePlayerShoot {
  int id;
};

struct MessageMobHit {
  int id;
  int hitX;
  int hitY;
};

struct MessageMobAttack {
  int mobId;
  int playerId;
};

struct MessageEntityUpdate {
  int id;
  float angle;
  Vector pos;
  Vector vel;
  int hp;
};

struct MessageGameState {
  uint32_t stamp;
  int nplayer;
  int nmob;
  MessageEntityUpdate entity[MAX_N_PLAYER + MAX_N_MOB];
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
ENetPacket *packageMessage(const T &message, MessageType type, bool reliable) {
  constexpr size_t headerSize = sizeof(MessageHeader);
  constexpr size_t messageSize = sizeof(T);
  constexpr size_t dataLen = headerSize + messageSize;
  std::array<char, dataLen> data{};
  serializeMessage(message, data.data(), type);
  ENetPacket *packet = enet_packet_create(
      data.data(), dataLen, reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
  return packet;
}

#endif
