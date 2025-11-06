#include "webserver_listcomponents.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/core/component_iterator.h"
#include "esphome/components/web_server_base/web_server_base.h"

#ifdef ARDUINO
#include "esphome/components/web_server/web_server.h"
#define WEBSERVER_HAS_ARDUINO 1
#else
#include "esphome/components/web_server_idf/web_server_idf.h"
#define WEBSERVER_HAS_IDF 1
#endif
#include <ArduinoJson.h>

// Per-entity includes guarded by USE_* like ESPHome does
#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif
#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif
#ifdef USE_SWITCH
#include "esphome/components/switch/switch.h"
#endif
#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif
#ifdef USE_LIGHT
#include "esphome/components/light/light_state.h"
#endif
#ifdef USE_CLIMATE
#include "esphome/components/climate/climate.h"
#endif
#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif
#ifdef USE_FAN
#include "esphome/components/fan/fan.h"
#endif
#ifdef USE_COVER
#include "esphome/components/cover/cover.h"
#endif
#ifdef USE_SELECT
#include "esphome/components/select/select.h"
#endif
#ifdef USE_BUTTON
#include "esphome/components/button/button.h"
#endif
#ifdef USE_LOCK
#include "esphome/components/lock/lock.h"
#endif
#ifdef USE_VALVE
#include "esphome/components/valve/valve.h"
#endif
#ifdef USE_MEDIA_PLAYER
#include "esphome/components/media_player/media_player.h"
#endif
#ifdef USE_ALARM_CONTROL_PANEL
#include "esphome/components/alarm_control_panel/alarm_control_panel.h"
#endif
#ifdef USE_EVENT
#include "esphome/components/event/event.h"
#endif
#ifdef USE_UPDATE
#include "esphome/components/update/update.h"
#endif
#ifdef USE_DATETIME_DATE
#include "esphome/components/datetime/date_entity.h"
#endif
#ifdef USE_DATETIME_TIME
#include "esphome/components/datetime/time_entity.h"
#endif
#ifdef USE_DATETIME_DATETIME
#include "esphome/components/datetime/datetime_entity.h"
#endif
#ifdef USE_TEXT
#include "esphome/components/text/text.h"
#endif

namespace esphome {
namespace webserver_listcomponents {

static const char *const TAG = "webserver_listcomponents";
static const char *const VER = "lc-endpoint v0.2.0";

// JSON-building iterator over all components
class ListComponentsJsonIterator : public esphome::ComponentIterator {
 public:
  using State = ComponentIterator::IteratorState;
  State get_state() const { return this->state_; }
  explicit ListComponentsJsonIterator(ArduinoJson::JsonArray &out) : out_(out) {}

#ifdef USE_SENSOR
  bool on_sensor(sensor::Sensor *e) override { add_(e, "sensor"); return true; }
#endif
#ifdef USE_BINARY_SENSOR
  bool on_binary_sensor(binary_sensor::BinarySensor *e) override { add_(e, "binary_sensor"); return true; }
#endif
#ifdef USE_SWITCH
  bool on_switch(switch_::Switch *e) override { add_(e, "switch"); return true; }
#endif
#ifdef USE_NUMBER
  bool on_number(number::Number *e) override { add_(e, "number"); return true; }
#endif
#ifdef USE_LIGHT
  bool on_light(light::LightState *e) override { add_(e, "light"); return true; }
#endif
#ifdef USE_CLIMATE
  bool on_climate(climate::Climate *e) override { add_(e, "climate"); return true; }
#endif
#ifdef USE_TEXT_SENSOR
  bool on_text_sensor(text_sensor::TextSensor *e) override { add_(e, "text_sensor"); return true; }
#endif
#ifdef USE_FAN
  bool on_fan(fan::Fan *e) override { add_(e, "fan"); return true; }
#endif
#ifdef USE_COVER
  bool on_cover(cover::Cover *e) override { add_(e, "cover"); return true; }
#endif
#ifdef USE_SELECT
  bool on_select(select::Select *e) override { add_(e, "select"); return true; }
#endif
#ifdef USE_BUTTON
  bool on_button(button::Button *e) override { add_(e, "button"); return true; }
#endif
#ifdef USE_LOCK
  bool on_lock(lock::Lock *e) override { add_(e, "lock"); return true; }
#endif
#ifdef USE_VALVE
  bool on_valve(valve::Valve *e) override { add_(e, "valve"); return true; }
#endif
#ifdef USE_MEDIA_PLAYER
  bool on_media_player(media_player::MediaPlayer *e) override { add_(e, "media_player"); return true; }
#endif
#ifdef USE_ALARM_CONTROL_PANEL
  bool on_alarm_control_panel(alarm_control_panel::AlarmControlPanel *e) override { add_(e, "alarm_control_panel"); return true; }
#endif
#ifdef USE_EVENT
  bool on_event(event::Event *e) override { add_(e, "event"); return true; }
#endif
#ifdef USE_UPDATE
  bool on_update(update::UpdateEntity *e) override { add_(e, "update"); return true; }
#endif
#ifdef USE_DATETIME_DATE
  bool on_date(datetime::DateEntity *e) override { add_(e, "date"); return true; }
#endif
#ifdef USE_DATETIME_TIME
  bool on_time(datetime::TimeEntity *e) override { add_(e, "time"); return true; }
#endif
#ifdef USE_DATETIME_DATETIME
  bool on_datetime(datetime::DateTimeEntity *e) override { add_(e, "datetime"); return true; }
#endif
#ifdef USE_TEXT
  bool on_text(text::Text *e) override { add_(e, "text"); return true; }
#endif

  bool on_end() override { return true; }

 private:
  template <typename T>
  void add_(T *entity, const char *type) {
    auto obj = out_.add<ArduinoJson::JsonObject>();
    obj["type"] = type;
    obj["object_id"] = entity->get_object_id();
    obj["name"] = entity->get_name();
  }

  ArduinoJson::JsonArray &out_;
};

#ifdef WEBSERVER_HAS_IDF
// ESP-IDF web server handler
class ListComponentsHandlerIDF : public esphome::web_server_idf::AsyncWebHandler {
 public:
  bool canHandle(esphome::web_server_idf::AsyncWebServerRequest *request) const override {
    const auto url = request->url();
    const bool match = (url == "/components" || url == "/components/");
    ESP_LOGD(TAG, "can_handle url=%s match=%d", url.c_str(), match);
    return match;
  }

  void handleRequest(esphome::web_server_idf::AsyncWebServerRequest *request) override {
    ESP_LOGD(TAG, "handle_request /components");

    ArduinoJson::JsonDocument doc;  // ArduinoJson v8
    auto root = doc.to<ArduinoJson::JsonObject>();
    auto arr = root["components"].to<ArduinoJson::JsonArray>();

    // Iterate all known components
    ListComponentsJsonIterator it(arr);
    it.begin();
    while (it.get_state() != ListComponentsJsonIterator::State::NONE) {
      it.advance();
    }

    std::string json;
    ArduinoJson::serializeJson(doc, json);
    request->send(200, "application/json", json.c_str());
  }
};
#elif defined(WEBSERVER_HAS_ARDUINO)
// Arduino (ESPAsyncWebServer) handler
class ListComponentsHandlerArduino : public AsyncWebHandler {
 public:
  bool canHandle(AsyncWebServerRequest *request) override {
    const auto url = request->url();
    const bool match = (url == "/components" || url == "/components/");
    ESP_LOGD(TAG, "can_handle url=%s match=%d", url.c_str(), match);
    return match;
  }

  void handleRequest(AsyncWebServerRequest *request) override {
    ESP_LOGD(TAG, "handle_request /components");

    ArduinoJson::JsonDocument doc;  // ArduinoJson v8
    auto root = doc.to<ArduinoJson::JsonObject>();
    auto arr = root["components"].to<ArduinoJson::JsonArray>();

    // Iterate all known components
    ListComponentsJsonIterator it(arr);
    it.begin();
    while (it.get_state() != ListComponentsJsonIterator::State::NONE) {
      it.advance();
    }

    String json;
    ArduinoJson::serializeJson(doc, json);
    request->send(200, "application/json", json);
  }
};
#endif

float WebServerListComponents::get_setup_priority() const {
  // After WiFi; server attaches handlers when ready.
  return setup_priority::AFTER_WIFI;
}

void WebServerListComponents::setup() {
  ESP_LOGI(TAG, "Registering /components endpoint (%s)", VER);
#if defined(WEBSERVER_HAS_ARDUINO)
  ESP_LOGI(TAG, "Backend: Arduino");
#elif defined(WEBSERVER_HAS_IDF)
  ESP_LOGI(TAG, "Backend: IDF");
#else
  ESP_LOGI(TAG, "Backend: unknown");
#endif
  auto *ws = esphome::web_server_base::global_web_server_base;
  if (!ws) {
    ESP_LOGE(TAG, "Web server not available; cannot register /components");
    return;
  }

#if defined(WEBSERVER_HAS_IDF)
  ws->add_handler(new ListComponentsHandlerIDF());
  ESP_LOGI(TAG, "Registered /components endpoint (IDF)");
#elif defined(WEBSERVER_HAS_ARDUINO)
  ws->add_handler(new ListComponentsHandlerArduino());
  ESP_LOGI(TAG, "Registered /components endpoint (Arduino)");
#else
  ESP_LOGW(TAG, "No supported web server backend headers found; endpoint not registered");
#endif
}

void WebServerListComponents::dump_config() { ESP_LOGI(TAG, "Component loaded. Route: /components"); }

}  // namespace webserver_listcomponents
}  // namespace esphome
