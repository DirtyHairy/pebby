#include <pebble.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "common.h"
#include "ui.h"
#include "event_log.h"
#include "communication.h"

static int sleeping = 0;

void up_single_click_handler(ClickRecognizerRef recognizer, void *context) {
	time_t t = time(NULL);
	
    ui_feed(t);
    persist_write_int(PERSIST_BOTTLE, t);
    log_event(EVENT_TYPE_FEED, t);

    LOG(APP_LOG_LEVEL_DEBUG, "feed %lu", (uint32_t)t);

    comm_transmit_events();
}

void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
    time_t t = time(NULL);

    ui_diaper_change(t);
    persist_write_int(PERSIST_DIAPER, t);
    log_event(EVENT_TYPE_DIAPER_CHANGE, t);

    LOG(APP_LOG_LEVEL_DEBUG, "diaper change %lu", (uint32_t)t);

    comm_transmit_events();
}

void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
    time_t t = time(NULL);

	if (sleeping) {
		persist_write_int(PERSIST_MOON_END, t);
        ui_sleep_stop(t);
        log_event(EVENT_TYPE_SLEEP_STOP, t);
		sleeping = 0;

        LOG(APP_LOG_LEVEL_DEBUG, "sleep start %lu", (uint32_t)t);
	} else {
		persist_write_int(PERSIST_MOON_START, t);
        persist_write_int(PERSIST_MOON_END, 0);
        ui_sleep_start(t);
        log_event(EVENT_TYPE_SLEEP_START, t);
		sleeping = 1;

        LOG(APP_LOG_LEVEL_DEBUG, "sleep stop %lu", (uint32_t)t);
	}

    comm_transmit_events();
}

void config_provider(void* context) {
	window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);
}

/***** Tick Events *****/

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
    ui_update();
}

/***** App *****/

static void deinit() {
    comm_deinit();
    ui_deinit();
    log_deinit();

    tick_timer_service_unsubscribe();
}

static void init() {
    log_init();
    ui_init(config_provider);
    comm_init();

    tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);

    if (persist_exists(PERSIST_MOON_START)) {
        sleeping = !(persist_exists(PERSIST_MOON_END) &&
                persist_read_int(PERSIST_MOON_END) > persist_read_int(PERSIST_MOON_START));
    } else {
        sleeping = 0;
    }
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
