import axios from "axios"

const BASE_GITHUB_URL = "https://raw.githubusercontent.com/mr-propya/Esp8266MqttClock/master/patterns"

const loadGitFile: Promise<string>  = (fileName: string)=> {
    const url = `${BASE_GITHUB_URL}${fileName}`
    return axios.get(url).then(response => JSON.stringify(response.data));
}

