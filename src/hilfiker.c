#include <tizen.h>
#include "hilfiker.h"

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;

	Evas_Object *second_hand;
	Evas_Object *minute_hand;
	Evas_Object *hour_hand;
	Evas_Object *background;

	Ecore_Animator *animator;

	char *res_path;

} appdata_s;

#define TEXT_BUF_SIZE 256

Eina_Bool update_watch_hands(void *data)
{
	appdata_s *ad = data;

	int msec = 0;
	int sec = 0;
	int min = 0;
	int hour = 0;
	double degree = 0.0f;
	watch_time_h watch_time;
	watch_time_get_current_time(&watch_time);
	time_t now;
	watch_time_get_utc_timestamp(watch_time, &now);

 	struct tm *ptm = localtime(&now);
 	min = ptm->tm_min;
 	hour = ptm->tm_hour;

	watch_time_get_second(watch_time, &sec);
	watch_time_get_millisecond(watch_time, &msec);

	watch_time_delete(watch_time);

	degree = sec * 1.025 * 6;
	degree += msec * 6 / 1000.0;
	if(sec + msec/1000.0 > 58.5) {
		degree = 0;
	}

	int x, y, w, h;
	evas_object_geometry_get(ad->second_hand, &x, &y, &w, &h);
	Evas_Map *map = evas_map_new(4);
	evas_map_util_points_populate_from_object(map, ad->second_hand);
	evas_map_util_rotate(map, degree, x + (w / 2), y + (h / 2));
	evas_object_map_set(ad->second_hand, map);
	evas_object_map_enable_set(ad->second_hand, EINA_TRUE);
	evas_map_free(map);

	degree = min * 6;
	degree += sec * 6 / 60.0;

	evas_object_geometry_get(ad->minute_hand, &x, &y, &w, &h);
	map = evas_map_new(4);
	evas_map_util_points_populate_from_object(map, ad->minute_hand);
	evas_map_util_rotate(map, degree, x + (w / 2), y + (h / 2));
	evas_object_map_set(ad->minute_hand, map);
	evas_object_map_enable_set(ad->minute_hand, EINA_TRUE);
	evas_map_free(map);

	degree = hour * 30;
	degree += min * 30 / 60.0;

	evas_object_geometry_get(ad->hour_hand, &x, &y, &w, &h);
	map = evas_map_new(4);
	evas_map_util_points_populate_from_object(map, ad->hour_hand);
	evas_map_util_rotate(map, degree, x + (w / 2), y + (h / 2));
	evas_object_map_set(ad->hour_hand, map);
	evas_object_map_enable_set(ad->hour_hand, EINA_TRUE);
	evas_map_free(map);

	return EINA_TRUE;
}

static void
create_base_gui(appdata_s *ad, int width, int height)
{
	int ret;
	char buff[TEXT_BUF_SIZE];

	ret = watch_app_get_elm_win(&ad->win);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get window. err = %d", ret);
		return;
	}

	evas_object_resize(ad->win, width, height);

	ad->conform = elm_conformant_add(ad->win);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	ad->background = elm_image_add(ad->conform);
	evas_object_resize(ad->background, 360, 360);
	evas_object_move(ad->background, 0, 0);
	snprintf(buff, TEXT_BUF_SIZE, "%s/background.png", app_get_resource_path());
	elm_image_file_set(ad->background, buff, NULL);
	evas_object_show(ad->background);

	ad->hour_hand = elm_image_add(ad->conform);
	evas_object_resize(ad->hour_hand, 360, 360);
	evas_object_move(ad->hour_hand, 0, 0);
	snprintf(buff, TEXT_BUF_SIZE, "%s/hourhand.png", app_get_resource_path());
	elm_image_file_set(ad->hour_hand, buff, NULL);
	evas_object_show(ad->hour_hand);

	ad->minute_hand = elm_image_add(ad->conform);
	evas_object_resize(ad->minute_hand, 360, 360);
	evas_object_move(ad->minute_hand, 0, 0);
	snprintf(buff, TEXT_BUF_SIZE, "%s/minutehand.png", app_get_resource_path());
	elm_image_file_set(ad->minute_hand, buff, NULL);
	evas_object_show(ad->minute_hand);

	ad->second_hand = elm_image_add(ad->conform);
	evas_object_resize(ad->second_hand, 360, 360);
	evas_object_move(ad->second_hand, 0, 0);
	snprintf(buff, TEXT_BUF_SIZE, "%s/secondhand.png", app_get_resource_path());
	elm_image_file_set(ad->second_hand, buff, NULL);
	evas_object_show(ad->second_hand);

	evas_object_show(ad->win);
}

static bool
app_create(int width, int height, void *data)
{
	appdata_s *ad = data;

	ad->res_path = app_get_resource_path();

	create_base_gui(ad, width, height);

	ad->animator = ecore_animator_add(update_watch_hands, ad);

	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
}

static void
app_pause(void *data)
{
}

static void
app_resume(void *data)
{
}

static void
app_terminate(void *data)
{
}

static void
app_time_tick(watch_time_h watch_time, void *data)
{
}

static void
app_ambient_tick(watch_time_h watch_time, void *data)
{
}

static void
app_ambient_changed(bool ambient_mode, void *data)
{
}

static void
watch_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	char *locale = NULL;
	app_event_get_language(event_info, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void
watch_app_region_changed(app_event_info_h event_info, void *user_data)
{
}

int
main(int argc, char *argv[])
{
	appdata_s ad = {0,};
	int ret = 0;

	watch_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;
	event_callback.time_tick = app_time_tick;
	event_callback.ambient_tick = app_ambient_tick;
	event_callback.ambient_changed = app_ambient_changed;

	watch_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
			APP_EVENT_LANGUAGE_CHANGED, watch_app_lang_changed, &ad);
	watch_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
			APP_EVENT_REGION_FORMAT_CHANGED, watch_app_region_changed, &ad);

	ret = watch_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_main() is failed. err = %d", ret);
	}

	return ret;
}

