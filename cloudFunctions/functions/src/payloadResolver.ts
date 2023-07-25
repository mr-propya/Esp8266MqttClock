import axios from "axios";

const BASE_GITHUB_URL = "https://raw.githubusercontent.com/mr-propya/Esp8266MqttClock/payload_resolvers/patterns/"

interface PayloadResolver{
    resolveParam: (att: string, contentMap: Map<String, any>) => Promise<string>;
    resolverRegexPattern: () => string;
}

const BASE_REGEX_TEMPLATE = (prefix: string)=>{
    return `\$\{${prefix}:([^\s\"\}]+)\}`
}
const getResolverData = (matchedGroup: string)=>{
    return matchedGroup.split(":", 2)[1].replace("}","")
}

const loadGitFile = (fileName: string)=> {
    const url = `${BASE_GITHUB_URL}${fileName}`
    return axios.get(url).then(response => JSON.stringify(response.data));
}


class ArgIfNotNull implements PayloadResolver{
    // MAP_ARG_IF_NULL:BRIGHTNESS?128
    resolveParam(att: string, contentMap: Map<String, any>): Promise<string> {
        const [key, defaultVal] = getResolverData(att).split("?", 2)
        if (Object.keys(contentMap).includes(key)){
            return Promise.resolve(contentMap.get(key))
        }
        return Promise.resolve(defaultVal);
    }
    resolverRegexPattern(): string {
        return BASE_REGEX_TEMPLATE("MAP_ARG_IF_NULL");
    }
}

class StaticTemplateResolvers implements PayloadResolver{
    // MAP_TEMPLATE:static_replacements/blink/uniform_blink
    resolveParam(att: string, contentMap: Map<String, any>):  Promise<string> {
        const templateName = getResolverData(att);
        return loadGitFile(templateName);
    }

    resolverRegexPattern(): string {
        return BASE_REGEX_TEMPLATE("MAP_TEMPLATE");
    }
}

class CharacterToBinaryResolver implements PayloadResolver{
    // RESOLVE_BINARY_SEGMENTS_ON:2,3,5,6,7
    resolveParam(att: string, contentMap: Map<String, any>): Promise<string> {
        const digitsOn = getResolverData(att).split(",").map(Number)
        let binaryValue = 0;
        for (let number of digitsOn) {
            binaryValue += 1<<number
        }
        return Promise.resolve(String(binaryValue));
    }

    resolverRegexPattern(): string {
        return BASE_REGEX_TEMPLATE("RESOLVE_BINARY_SEGMENTS_ON");
    }

}

const resolvers : Array<PayloadResolver> = [
    new CharacterToBinaryResolver(),
    new ArgIfNotNull(),
    new StaticTemplateResolvers()
]

const resolveString = async (content: string, context: Map<string, any>, visitedPattens: Set<string>) : Promise<string> =>{
    for (let resolver of resolvers) {
        const matches: null|RegExpMatchArray = new RegExp(resolver.resolverRegexPattern()).exec(content);
        if(matches==null)
            continue

        for (let s in matches) {
            if (visitedPattens.has(s))
                throw new Error(`Template has recursive pattern: ${s}`)

            let finalReplacement = await resolver.resolveParam(s, context)
            visitedPattens.add(s)
            finalReplacement = await resolveString(finalReplacement, context, visitedPattens)
            visitedPattens.delete(s)
            content = content.replace(s, finalReplacement)
        }
    }
    return content
}

export const getPreparedTemplate = async (templateName: string, context: Map<string, any>) : Promise<string>  =>{
    const templateText = await loadGitFile(`templates/${templateName}.template`)
    return resolveString(templateText, context, new Set())
}