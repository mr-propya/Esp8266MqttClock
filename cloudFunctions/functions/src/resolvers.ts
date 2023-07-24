interface PayloadResolver{
    resolveParam: (att: string, contentMap: Map<String, any>) => Promise<string>;
    resolverRegexPattern: () => string;
}

const BASE_REGEX_TEMPLATE = (prefix: string)=>{
    return `${prefix}:([^\\s\\"]+:*)+`
}

class ArgIfNotNull implements PayloadResolver{
    resolveParam(att: string, contentMap: Map<String, any>): Promise<string> {
        return Promise.resolve("");
    }
    resolverRegexPattern(): string {
        return BASE_REGEX_TEMPLATE("MAP_ARG_IF_NULL");
    }
}

class StaticTemplateResolvers implements PayloadResolver{
    resolveParam(att: string, contentMap: Map<String, any>):  Promise<string> {
        return Promise.resolve("");
    }

    resolverRegexPattern(): string {
        return BASE_REGEX_TEMPLATE("MAP_TEMPLATE");
    }
}

class CharacterToBinaryResolver implements PayloadResolver{
    resolveParam(att: string, contentMap: Map<String, any>): Promise<string> {
        const digitsOn = att.split(":", 2)[1].split(",").map(Number)
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