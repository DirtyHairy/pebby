#ifndef EVENT_LOG_H
#define EVENT_LOG_H

#include <pebble.h>

#include "common.h"

#define EVENT_LOG_MAX_SIZE 50
#define SERIALIZED_EVENT_SIZE 5

typedef enum {
    EVENT_TYPE_FEED = 0,
    EVENT_TYPE_DIAPER_CHANGE = 1,
    EVENT_TYPE_SLEEP_START = 2,
    EVENT_TYPE_SLEEP_STOP = 3
} EventType;

typedef struct {
    EventType type;
    time_t timestamp;
} Event;

uint8_t event_log_size();

bool log_full();

bool log_event(EventType type, time_t timestamp);

Event* log_get_event(uint8_t index);

void log_purge_from_start(uint8_t count);

void log_serialize_event(Event* event, uint8_t *buffer);

uint8_t log_calculate_serialized_size();

void log_serialize(uint8_t *buffer);

bool log_unserialize(const uint8_t *buffer);

void log_init();

void log_deinit();

void log_reset();

#endif // EVENT_LOG_H
