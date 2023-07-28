import {getPreparedTemplate} from "./payloadResolver";

const main = async ()=>{
    const context = new Map();
    context.set("DEVICE_ID","test")
    context.set("BRIGHTNESS","124")
    const response = await getPreparedTemplate("hi_pallete", context)
    console.log(response)
}

main()