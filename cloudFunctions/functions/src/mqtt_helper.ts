import * as mqtt from "mqtt"

const protocol = 'mqtts'
const host = '43d985e2b31746e6980bc97ba5b1d77d.s1.eu.hivemq.cloud'
const port = '8883'
const clientId = `mqtt_server_${Date().toString()}`

const connectUrl = `${protocol}://${host}:${port}`


export const publxishMqtt = async (topic: string, payload: string) : Promise<any> =>{
    return new Promise((resolve, reject)=>{
        const client = mqtt.connect(connectUrl, {
            clientId,
            clean: true,
            connectTimeout: 4000,
            username: 'nodeUser',
            password: 'nodePasswd',
            reconnectPeriod: 1000,
        })
        client.publish(topic, payload, (err)=>{
            if(err !=null){
                reject(err)
            }
            client.end((err)=>{
                if(err!=null)
                    reject(err)
                resolve({
                    "status": "success",
                    "topic": topic,
                    "payload": payload
                })
            })
        })
    })

}
