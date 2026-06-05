#ifndef BOTCHLING_HPP
#define BOTCHLING_HPP

#include "common/mmo.hpp"
#include "map/map.hpp"
#include <cstdint>

enum e_botchling_event_type : uint8_t {
  BOTCHLING_EVENT_NONE = 0,
  BOTCHLING_EVENT_TYPE_LOGIN = 1,
  BOTCHLING_EVENT_TYPE_LOGOUT = 2,
  BOTCHLING_EVENT_TYPE_WALK = 3,
  BOTCHLING_EVENT_TYPE_TP = 4,
  BOTCHLING_EVENT_TYPE_MAP_CHG = 5,
  BOTCHLING_EVENT_TYPE_MOB_KILL = 6,
  BOTCHLING_EVENT_TYPE_TAKE_ITEM = 7,
	BOTCHLING_EVENT_TYPE_CHAT_SENT = 8,
  BOTCHLING_EVENT_TYPE_MAX_VALUE = 9,
};

#pragma pack(push, 1)
struct s_botchling_event {
	uint8_t event_type;
  int64_t timestamp;
  uint32_t account_id;
  uint32_t char_id;
  union {
    struct {
      uint32_t ip;
      char map[MAP_NAME_LENGTH_EXT];
    } login;
    struct {
      int16_t from_x;
      int16_t from_y;
      int16_t to_x;
      int16_t to_y;
      char map[MAP_NAME_LENGTH_EXT];
    } walk;
    struct {
      int16_t x;
      int16_t y;
      char map[MAP_NAME_LENGTH_EXT];
    } tp;
    struct {
      int16_t x;
      int16_t y;
      char from_map[MAP_NAME_LENGTH_EXT];
      char to_map[MAP_NAME_LENGTH_EXT];
    } map_chg;
    struct {
      uint16_t mob_id;
      int16_t x;
      int16_t y;
      char map[MAP_NAME_LENGTH_EXT];
    } mob_kill;
    struct {
      uint32_t item_id;
      uint16_t amount;
      char map[MAP_NAME_LENGTH_EXT];
    } take_item;
		struct {
			char message[CHAT_SIZE_MAX];
			char map[MAP_NAME_LENGTH_EXT];
		} chat_sent;
  } event_data;
};
#pragma pack(pop)

bool botchling_init(const char *socket_path);
void botchling_shutdown();

// emmiters
void botchling_emit_login(uint32_t account_id, uint32_t char_id, uint32_t ip, const char *map);
void botchling_emit_logout(uint32_t account_id, uint32_t char_id);
void botchling_emit_walk(uint32_t account_id, uint32_t char_id, int16_t from_x, int16_t from_y, int16_t to_x, int16_t to_y, const char *map);
void botchling_emit_tp(uint32_t account_id, uint32_t char_id, int16_t x, int16_t y, const char *map);
void botchling_emit_map_chg(uint32_t account_id, uint32_t char_id, int16_t x, int16_t y, const char *from_map, const char *to_map);
void botchling_emit_mob_kill(uint32_t account_id, uint32_t char_id, uint16_t mob_id, int16_t x, int16_t y, const char *map);
void botchling_emit_take_item(uint32_t account_id, uint32_t char_id, uint32_t item_id, uint16_t amount, const char *map);
void botchling_emit_chat_sent(uint32_t account_id, uint32_t char_id, const char *message, const char *map);

#endif /* BOTCHLING_HPP */

