import {getPreparedTemplate} from "./payloadResolver";

const main = async ()=>{
    const response = await getPreparedTemplate("hi_pallete", new Map())
    console.log(response)
}

main()