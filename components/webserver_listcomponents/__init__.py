from esphome import core
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

DEPENDENCIES = ["web_server"]         # ensures web_server is enabled in YAML
AUTO_LOAD = ["web_server_base"]       # ensures base types are built

ns = cg.esphome_ns.namespace("webserver_listcomponents")
WebServerListComponents = ns.class_("WebServerListComponents", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(WebServerListComponents),
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    # ESPAsyncWebServer is an Arduino-only library. The IDF backend
    # (web_server_idf) does not use it, so only link it on Arduino.
    if core.CORE.using_arduino:
        cg.add_library("esphome/ESPAsyncWebServer-esphome", None)
