import * as functions from "firebase-functions";
import {database} from "firebase-admin";
import {initializeApp} from "firebase-admin/app";
import {getPreparedTemplate} from "./payloadResolver"
initializeApp();

//copyTemplate
//getData

interface DataMsg {
  firstDisplayed?: number,
  showSec: number,
  msgString: string
}

interface TemplateData {
  deviceId: string,
  msgStrings: string[],
  showSec: number,
  friendlyDesc? : string
}

export const writeTemplate = functions.https.onRequest(async (request, response) => {
  const templateId = request.query["templateId"];
  functions.logger.info(`Getting template for template id ${templateId}`, {structuredData: true});
  const dbItem = await database().ref(`templates/${templateId}`).get();
  if(!dbItem.exists()){
    response.send("INVALID_TEMPLATE");
    return;
  }
  const templateData: TemplateData = dbItem.val();
  for (const msg of templateData.msgStrings) {
    const msgData: DataMsg = {
      showSec: templateData.showSec,
      msgString: msg,
    };
    await database().ref(`devices/${templateData.deviceId}/messages`).push(msgData);
  }
  response.send(`Sent data ${templateData.friendlyDesc || ""}`);
});


export const getData = functions.https.onRequest(async (request, response) => {
  const deviceId = request.query["deviceId"];
  functions.logger.info(`Getting data for device id ${deviceId}`, {structuredData: true});
  while(true){
    const databaseItem = await database().ref(`devices/${deviceId}/messages`).orderByKey().limitToFirst(1).get();
    if(!databaseItem.exists()){
      response.send("NO_DATA");
      return
    }
    let childName = "dummy";
    databaseItem.forEach(children=>{
      childName = children.key || ""
    })
    const msg: DataMsg = databaseItem.child(childName).val();

    functions.logger.info(`Got data for device id ${deviceId} as ${JSON.stringify(msg)}`, {structuredData: true});

    if(msg.firstDisplayed != null && (msg.firstDisplayed + msg.showSec * 1000) < new Date().getTime()){
        await databaseItem.ref.child(childName).remove();
    }else{
      if(msg.firstDisplayed == null){
        await databaseItem.ref.child(childName).update({
          firstDisplayed : new Date().getTime()
        })
      }
      response.send(msg.msgString);
      return
    }
  }
});

export const getTemplate = functions.https.onRequest(async (request, response)=>{
  const context = {
    ...request.query,
    ...request.body
  }
  try {
    response.send({
      "status":"success",
      "payload": JSON.stringify(await getPreparedTemplate(context["templateName"], context))
    })
  }catch (e){
    response.statusCode = 500
    response.send({
      "status":"error",
      "error": JSON.stringify(e)
    })
  }


})