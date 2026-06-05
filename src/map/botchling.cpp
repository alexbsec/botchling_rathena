#include "botchling.hpp"
#include "common/showmsg.hpp"
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

static int g_fd = -1;
static struct sockaddr_un g_addr;
static const char *SOCKET_PATH = "/var/run/botchling/botchling.sock";

static int64_t botchling_get_timestamp() {
  struct timespec ts{};
  clock_gettime(CLOCK_REALTIME, &ts);
  return ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL;
}

static bool botchling_emit_event(e_botchling_event_type event_type,
                                 uint32_t account_id, uint32_t char_id,
                                 const void *event_data, size_t data_size) {
  if (g_fd < 0) {
    return false;
  }

  s_botchling_event event{};
  event.event_type = event_type;
  event.account_id = account_id;
  event.char_id = char_id;
  event.timestamp = botchling_get_timestamp();
  if (event_data && data_size > 0) {
    memcpy(&event.event_data, event_data, data_size);
  }

  sendto(g_fd, &event, sizeof(event), MSG_DONTWAIT, (struct sockaddr *)&g_addr,
         sizeof(g_addr));
  return true;
}

bool botchling_init(const char *socket_path) {
  g_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (g_fd < 0) {
    ShowError("botchling_init: Failed to create socket\n");
    return false;
  }

  int flags = fcntl(g_fd, F_GETFL, 0);
  fcntl(g_fd, F_SETFL, flags | O_NONBLOCK);

  memset(&g_addr, 0, sizeof(g_addr));
  g_addr.sun_family = AF_UNIX;
  strncpy(g_addr.sun_path, socket_path ? socket_path : SOCKET_PATH,
          sizeof(g_addr.sun_path) - 1);
	ShowInfo("botchling_init: Initialized with socket path: %s\n", g_addr.sun_path);
  return true;
}

void botchling_shutdown() {
  if (g_fd >= 0) {
    close(g_fd);
    g_fd = -1;
  }
}

void botchling_emit_login(uint32_t account_id, uint32_t char_id, uint32_t ip,
                          const char *map) {
  struct {
    uint32_t ip;
    char map[MAP_NAME_LENGTH_EXT];
  } data{};

  data.ip = ip;
  strncpy(data.map, map, sizeof(data.map) - 1);
  botchling_emit_event(BOTCHLING_EVENT_TYPE_LOGIN, account_id, char_id, &data,
                       sizeof(data));
}

void botchling_emit_logout(uint32_t account_id, uint32_t char_id) {
	botchling_emit_event(BOTCHLING_EVENT_TYPE_LOGOUT, account_id, char_id, nullptr,
											 0);
}

void botchling_emit_walk(uint32_t account_id, uint32_t char_id, int16_t from_x,
													int16_t from_y, int16_t to_x, int16_t to_y,
													const char *map) {
	struct {
		int16_t from_x;
		int16_t from_y;
		int16_t to_x;
		int16_t to_y;
		char map[MAP_NAME_LENGTH_EXT];
	} data{};

	data.from_x = from_x;
	data.from_y = from_y;
	data.to_x = to_x;
	data.to_y = to_y;
	strncpy(data.map, map, sizeof(data.map) - 1);
	botchling_emit_event(BOTCHLING_EVENT_TYPE_WALK, account_id, char_id, &data,
											 sizeof(data));
}

void botchling_emit_tp(uint32_t account_id, uint32_t char_id, int16_t x,
											 int16_t y, const char *map) {
	struct {
		int16_t x;
		int16_t y;
		char map[MAP_NAME_LENGTH_EXT];
	} data{};

	data.x = x;
	data.y = y;
	strncpy(data.map, map, sizeof(data.map) - 1);
	botchling_emit_event(BOTCHLING_EVENT_TYPE_TP, account_id, char_id, &data,
											 sizeof(data));
}

void botchling_emit_map_chg(uint32_t account_id, uint32_t char_id, int16_t x,
													 int16_t y, const char *from_map, const char *to_map) {
	struct {
		int16_t x;
		int16_t y;
		char from_map[MAP_NAME_LENGTH_EXT];
		char to_map[MAP_NAME_LENGTH_EXT];
	} data{};

	data.x = x;
	data.y = y;
	strncpy(data.from_map, from_map, sizeof(data.from_map) - 1);
	strncpy(data.to_map, to_map, sizeof(data.to_map) - 1);
	botchling_emit_event(BOTCHLING_EVENT_TYPE_MAP_CHG, account_id, char_id, &data,
											 sizeof(data));
}

void botchling_emit_mob_kill(uint32_t account_id, uint32_t char_id, uint16_t mob_id,
													 int16_t x, int16_t y, const char *map) {
	struct {
		uint16_t mob_id;
		int16_t x;
		int16_t y;
		char map[MAP_NAME_LENGTH_EXT];
	} data{};

	data.mob_id = mob_id;
	data.x = x;
	data.y = y;
	strncpy(data.map, map, sizeof(data.map) - 1);
	botchling_emit_event(BOTCHLING_EVENT_TYPE_MOB_KILL, account_id, char_id, &data,
											 sizeof(data));
}

void botchling_emit_take_item(uint32_t account_id, uint32_t char_id, uint32_t item_id,
													 uint16_t amount, const char *map) {
	struct {
		uint32_t item_id;
		uint16_t amount;
		char map[MAP_NAME_LENGTH_EXT];
	} data{};

	data.item_id = item_id;
	data.amount = amount;
	strncpy(data.map, map, sizeof(data.map) - 1);
	botchling_emit_event(BOTCHLING_EVENT_TYPE_TAKE_ITEM, account_id, char_id, &data,
											 sizeof(data));
}

void botchling_emit_chat_sent(uint32_t account_id, uint32_t char_id, const char *message,
														 const char *map) {
	struct {
		char message[CHAT_SIZE_MAX];
		char map[MAP_NAME_LENGTH_EXT];
	} data{};

	strncpy(data.message, message, sizeof(data.message) - 1);
	strncpy(data.map, map, sizeof(data.map) - 1);
	botchling_emit_event(BOTCHLING_EVENT_TYPE_CHAT_SENT, account_id, char_id, &data,
											 sizeof(data));
}
