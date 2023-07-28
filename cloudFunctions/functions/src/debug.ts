import {getPreparedTemplate} from "./payloadResolver";
import {publishMqtt} from "./mqtt_helper";

const main = async ()=>{
    const context = new Map();
    context.set("DEVICE_ID","Test")
    context.set("BRIGHTNESS","124")
    context.set("CUSTOM_DATA_PARAM",
        // "${RESOLVE_BINARY_SEGMENTS_ON:1}," +
        // "${RESOLVE_BINARY_SEGMENTS_ON:2}," +
        "${MAP_TEMPLATE:static_replacements/chars/H}," +
        "${MAP_TEMPLATE:static_replacements/chars/E}," +
        "${MAP_TEMPLATE:static_replacements/chars/Y}," +
        // "${MAP_TEMPLATE:static_replacements/chars/Y}," +
        // "${MAP_TEMPLATE:static_replacements/chars/L}," +
        // "${MAP_TEMPLATE:static_replacements/chars/L}," +
        // "${MAP_TEMPLATE:static_replacements/chars/O}," +
        "${MAP_TEMPLATE:static_replacements/chars/Y}"
        // "${MAP_TEMPLATE:static_replacements/chars/O}," +
        // "${MAP_TEMPLATE:static_replacements/chars/1}," +
        // "${MAP_TEMPLATE:static_replacements/chars/2}," +
        // "${MAP_TEMPLATE:static_replacements/chars/3}," +
        // "${MAP_TEMPLATE:static_replacements/chars/4}," +
        // "${MAP_TEMPLATE:static_replacements/chars/5}," +
        // "${MAP_TEMPLATE:static_replacements/chars/6}," +
        // "${MAP_TEMPLATE:static_replacements/chars/7}," +
        // "${MAP_TEMPLATE:static_replacements/chars/8}," +
        // "${MAP_TEMPLATE:static_replacements/chars/9}," +
        // "${MAP_TEMPLATE:static_replacements/chars/5}"
    )
    const response = await getPreparedTemplate("custom_data", context)
    console.log(response)
    await publishMqtt(JSON.parse(response)["topic"],response)
}
main()
