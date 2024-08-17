#include <DFRobot_WiFi_IoT_Module.h>
#include <string.h>


char *_topicName[5];

uint8_t DFRobot_WiFi_IoT_Module_I2C::begin(void)
{
  uint8_t buffer[2];
  _pWire->begin();

  if(readReg(0,buffer, 2) == 2 ){
    clearBuffer();
    DBG("WIFI_IOT_OK");
    return WIFI_IOT_OK;
  }
  else {
    DBG("WIFI_IOT_ERROR!");
    return WIFI_IOT_ERROR;
  }
}

uint8_t DFRobot_WiFi_IoT_Module_I2C::connectWifi(char* ssid, char* pwd)
{
  manageFunction(IOT_SET_COMMAND, SET_WIFI_NAME, ssid);
  manageFunction(IOT_SET_COMMAND, SET_WIFI_PASSWORD, pwd);
  connection(CONNECT_WIFI);
  return getWifiState();
}

uint8_t DFRobot_WiFi_IoT_Module_I2C::MQTTBegin(char *server, char *port, char *productID, char *pwd, char* deviceID)
{
  manageFunction(IOT_SET_COMMAND, SET_MQTT_SERVER, server);
  manageFunction(IOT_SET_COMMAND, SET_MQTT_PORT, port);
  manageFunction(IOT_SET_COMMAND, SET_MQTT_ID, productID);
  manageFunction(IOT_SET_COMMAND, SET_MQTT_PASSWORD, pwd);

  if(deviceID == NULL){
    connection(CONNECT_MQTT);
  }else{
    manageFunction(IOT_SET_COMMAND, SET_MQTT_DEVICEID, deviceID);
    connection(CONNECT_MQTT_ONENET);
  }

  return getMQTTState();
}

uint8_t DFRobot_WiFi_IoT_Module_I2C::subscribe(char *topic)
{
  char *_topic = topic;
  static uint8_t num = 0, subscribeTopic = 0;
  if(num>5){
    return FULL_SUBSCRIPTINO;
  }

  for(uint8_t i = 0; i < 5; i++){
    if(strcmp(_topicName[i],_topic) == 0){
      return REPEAT_SUBSCRIPTION;
    }
  }
  _topicName[num] = _topic;
  DBG(_topicName[num]);
  switch(num){
  case TOPIC0:
    subscribeTopic = 0x06;
    num++;
    break;
  case TOPIC1:
    subscribeTopic = 0x07;
    num++;
    break;
  case TOPIC2:
    subscribeTopic = 0x08;
    num++;
    break;
  case TOPIC3:
    subscribeTopic = 0x09;
    num++;
    break;
  case TOPIC4:
    subscribeTopic = 0x0A;
    num++;
    break;
  default:
    return RETURN_NONE;
  }
  manageFunction(IOT_RUN_COMMAND, subscribeTopic, _topic);//Subscribe to topic

  if(strncmp(topic,"$dp",3) ==0){
    return 0;
  }
  return getTopicState();
}

void DFRobot_WiFi_IoT_Module_I2C::clearBuffer(void)
{
  uint8_t buf[10];
  buf[0] = 0x02;
  buf[1] = 0x17;
  writeReg(0x1E, buf, (uint8_t)2);
  //Serial.println("clear buffer!");
  delay(1000);
}

uint8_t DFRobot_WiFi_IoT_Module_I2C::publish(char *topic, String data)
{
  uint8_t ret = -1;
  char *_topic = topic;
  if((!data) || (!_topic)){
    ret = 3;
  }else{
    uint8_t num =254, sendTopic = 0;
    for(uint8_t i = 0; i < 5;i++){
      if(strcmp(_topicName[i],_topic) == 0){
        num = i;
        break;
      }
    }
    DBG(num);
    switch(num){
    case TOPIC0:
      sendTopic = 0x0B;
      break;
    case TOPIC1:
      sendTopic = 0x0C;
      break;
    case TOPIC2:
      sendTopic = 0x0D;
      break;
    case TOPIC3:
      sendTopic = 0x0E;
      break;
    case TOPIC4:
      sendTopic = 0x0F;
      break;
    default:
      ret = 1;
    }
    manageFunction(IOT_RUN_COMMAND, sendTopic, data);
  }
  uint32_t startingTime = millis();
  while(1){
    uint32_t currentTime = millis();
    delay(100);
    if(getMQTTSendState() == 1){
      ret = 0;
      break;
    }else if((currentTime-startingTime) > 100000){
      ret = -1;
      break;
    }
  }
  return ret;
}

void DFRobot_WiFi_IoT_Module_I2C::manageFunction(uint8_t command, uint8_t config, String data)
{
  uint8_t i = 0, j = 0;
  i = data.length();
  uint8_t datalen = i+3;
  uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t)*datalen);
  if(buffer == NULL){
    Serial.print("memory allocation failed");
    return;
  }
  buffer[0]  =  command;
  buffer[1]  =  config;
  buffer[2]  =  datalen-3;
  for(i = 3,j = 0; i < datalen; i++){
    buffer[i] = (char)data[j++];
  } 
  writeReg(IOT_COMMAND_REGTSTER, buffer, datalen);
  free(buffer);
}

// void DFRobot_WiFi_IoT_Module_I2C::manageFunction(uint8_t command, uint8_t config, uint8_t *data, uint16_t len)
// {
//   uint8_t i = 0, j = 0;
//   uint8_t datalen = len+3;
//   uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t)*datalen);
//   if(buffer == NULL){
//     Serial.print("memory allocation failed");
//     return;
//   }
//   buffer[0]  =  command;
//   buffer[1]  =  config;
//   buffer[2]  =  len;
//   for(i = 3,j = 0; i < datalen; i++,j++){
//     buffer[i] = data[j];
//   } 

//   writeReg(IOT_COMMAND_REGTSTER, buffer, datalen);
//   free(buffer);
// }

void DFRobot_WiFi_IoT_Module_I2C::reconnectWifi()
{
  connection(RECONNECT_WIFI);
  while(1){
    if(getWifiState() == 1){
      break;
    }
  }
  _wifiState = MQTT_SUCCESS_CON;
}

bool DFRobot_WiFi_IoT_Module_I2C::disconnectWifi()
{
  connection(BREAK_WIFI_CON);
  while(1){
    if(getWifiState() == 1){
      break;
    }
  }
  _wifiState = WIFI_NO_CON;
  return true;
}

uint8_t DFRobot_WiFi_IoT_Module_I2C::getWifiState()
{
  uint8_t buffer[2]={0};
  if(readReg(IOT_COMMAND_REGTSTER, &buffer, 2) != 2){
    DBG("READ WIFI_IOT_ERROR!!!!!!");
    return WIFI_IOT_ERROR;
  }else{
    if(buffer[0] == WIFI_STATUE_CON){
      if(buffer[1] == WIFI_SUCCESS_CON){
        DBG("WIFI SUCCESS CON");
        _wifiState = WIFI_SUCCESS_CON;            //WiFi connected
        return 0;
      }else if(buffer[1] == WIFI_NO_CON){
        DBG("WIFI NO CON");
        _wifiState = WIFI_NO_CON;                 //Wifi connection error
        return 1;
      }else if(buffer[1] == WIFI_RUN_CON){
        DBG("WIFI RUN CON");
        _wifiState = WIFI_RUN_CON;               //Connecting to WiFi
        return 2;
      }else{
        DBG("WIFI WIFI_IOT_ERROR");
        return WIFI_IOT_ERROR;
      }
    }else{
      DBG("NULL");
      return WIFI_IOT_ERROR;
    }
  }
  return WIFI_IOT_ERROR;
}

uint8_t DFRobot_WiFi_IoT_Module_I2C::getMQTTState()
{
  uint8_t buffer[2];
  if(readReg(IOT_COMMAND_REGTSTER, &buffer, 2) != 2){
    DBG("READ WIFI_IOT_ERROR!!!!!!");
    return WIFI_IOT_ERROR;
  }else{
    DBG(buffer[0]);
    if(buffer[0] == MQTT_STATUE_CON){
      if(buffer[1] == MQTT_SUCCESS_CON){
        DBG("MQTT_SUCCESS_CON");
        return 0;
      }else if(buffer[1] == MQTT_ERROR_CON){
        DBG("MQTT_ERROR_CON");
        return 1;
      }else if(buffer[1] == MQTT_START){
        DBG("MQTT_START");
        return 2;
      }
    }else if(buffer[0] == NONE){
      DBG("RETURN_NONE");
      return RETURN_NONE;
    }else{
      DBG("WIFI_IOT_ERROR");
      return WIFI_IOT_ERROR;
    }
  }
  return WIFI_IOT_ERROR;
}

uint8_t DFRobot_WiFi_IoT_Module_I2C::getMQTTSendState()
{
  uint8_t buffer[2];
  if(readReg(IOT_COMMAND_REGTSTER, &buffer, 2) != 2){
    DBG("READ WIFI_IOT_ERROR!!!!!!");
    return WIFI_IOT_ERROR;
  }else{
    DBG(buffer[0]);
    if(buffer[0] == GET_MQTTSEND_STATUE){
      if(buffer[1] == MQTT_SEND_SUCCESS){
        DBG("MQTT_SEND_SUCCESS");
        return 1;
      }else if(buffer[1] == MQTT_SEND_ERROR){
        DBG("MQTT_SEND_ERROR");
        return 2;
      }else{
        DBG("ERROR");
        return 0;
      }
    }
  }
  return WIFI_IOT_ERROR;
}

uint8_t DFRobot_WiFi_IoT_Module_I2C::getTopicState()
{
  uint8_t buffer[2] ;
  if(readReg(IOT_COMMAND_REGTSTER, &buffer, 2) != 2){
    DBG("READ WIFI_IOT_ERROR!!!!!!");
    return WIFI_IOT_ERROR;
  }else{
    if(buffer[0] == SUBSCRIBE_STATUE){
      switch(buffer[1]){
      case TAKE_SUCCESS:
        return 0;
        break;
      case TAKE_ERROR:
        return 1;
        break;
      case TAKE_UPPER_LIMIT:
        return 2;
        break;
      default:
        return 3;
      }
    }else if(buffer[0] == NONE){
      return RETURN_NONE;
    }else{
      return WIFI_IOT_ERROR;
    }
  }
  return WIFI_IOT_ERROR;
}

void DFRobot_WiFi_IoT_Module_I2C::connection(uint8_t command)
{
  uint8_t buffer[2];
  buffer[0] = 0x02;
  buffer[1] = command;
  writeReg(IOT_COMMAND_REGTSTER, &buffer, 2);	
}

String DFRobot_WiFi_IoT_Module_I2C::getVersion()
{
  uint8_t getVersionData[5];
  connection(QUERY_VERSION);
  delay(10);
  uint8_t state = parameterReturn(GET_VERSION, 0x00, &getVersionData[0]);
  return (char*)getVersionData;
}

// void DFRobot_WiFi_IoT_Module_I2C::setCallBack(callback call)
// {
//   this->_callback = call;
// }

uint8_t DFRobot_WiFi_IoT_Module_I2C::loop()
{
  uint8_t i = 0, topic;
  uint8_t buffer[3];
  if(readReg(IOT_COMMAND_REGTSTER, &buffer, 3) != 3){
    DBG("READ WIFI_IOT_ERROR");
    return WIFI_IOT_ERROR;
  }else{
    if(buffer[0] == PING_STATUE){
    }else if(buffer[0] == WIFI_STATUE_CON){
      _wifiState = buffer[2];
    }else if(buffer[0] == RECV_TOPIC0){
      topic = 0;
    }else if(buffer[0] == RECV_TOPIC1){
      topic = 1;
    }else if(buffer[0] == RECV_TOPIC2){
     topic = 2;
    }else if(buffer[0] == RECV_TOPIC3){
      topic = 3;
    }else if(buffer[0] == RECV_TOPIC4){
      topic = 4;
    }else{
      return 0;
    }
    uint8_t datalen = buffer[1];
    if(datalen > 0x7F){
      return 0;
    }
    uint8_t *data = (uint8_t *)malloc(sizeof(uint8_t)*datalen);
    if(data == NULL){
      Serial.print("memory allocation failed");
      return 0;
    }
    // if(_callback){
    //   if(getData(READ_DATA_REGISTER, data, datalen) != datalen){
    //     DBG("READ WIFI_IOT_ERROR");
    //     return WIFI_IOT_ERROR;
    //   }else{
    //     _callback(_topicName[topic], data);
    //     free(data);
    //   }
    // }
  }
  return 1;
}

void DFRobot_WiFi_IoT_Module_I2C::writeReg(uint8_t reg, void *pBuf, size_t size)
{
  uint8_t *_pBuf = (uint8_t*)pBuf;
  _pWire->beginTransmission(_address);
  _pWire->write(reg);
  if(size > 32){
    uint16_t j = 1;
    for(uint8_t i = 0; i<size; i++){
      if(i >= (31*j)){
        _pWire->endTransmission(false);
        _pWire->beginTransmission(_address); 
        j++;
      }
      _pWire->write(_pBuf[i]);
    }
  }else{
    DBG("D");
    for(size_t i = 0; i < size; i++){
      _pWire->write(_pBuf[i]);
    }
  }
  _pWire->endTransmission();
  delay(20);
}

uint8_t DFRobot_WiFi_IoT_Module_I2C::readReg(uint8_t reg, void *pBuf, size_t size){
  if(pBuf == NULL){
    DBG("pBuf WIFI_IOT_ERROR!!!!");
    return 0;
  }
  uint8_t *_pBuf = (uint8_t*)pBuf;
  _pWire->beginTransmission(_address);
  _pWire->write(reg);
  _pWire->write(IOT_READ_COMMAND);
  _pWire->write(READ_COMMAND);
  if(_pWire->endTransmission() != 0){
    return 0;
  }
  delay(100);
  _pWire->requestFrom(_address,size);
  for(size_t i = 0; i < size; i++){
    _pBuf[i] = _pWire->read();
  }
  return size;
}

uint8_t DFRobot_WiFi_IoT_Module_I2C::getData(uint8_t reg, void *pBuf, size_t size)
{
  size_t i = 0;
  if(pBuf == NULL){
    DBG("pBuf WIFI_IOT_ERROR!!!!");
    return 0;
  }
  uint8_t *_pBuf = (uint8_t*)pBuf;
  if(size < 200){
    _pWire->beginTransmission(_address);
    _pWire->write(reg);
    if(_pWire->endTransmission() != 0){
      return 0;
    }
    _pWire->requestFrom(_address,size);
    for(i = i; i < size; i++){
      _pBuf[i] = _pWire->read();
    }
  }else{
    for(uint8_t j = 0; j < size / 200; j++){
      _pWire->beginTransmission(_address);
      _pWire->write(reg);
      if(_pWire->endTransmission() != 0){
        return 0;
      }
      _pWire->requestFrom(_address,size);
      for(i = i; i < 200; i++){
        _pBuf[i] = _pWire->read();
      }
    }
    _pWire->beginTransmission(_address);
    _pWire->write(reg);
    if(_pWire->endTransmission() != 0){
      return 0;
    }
    _pWire->requestFrom(_address,size%200);
    for(i = i; i < size%200; i++){
      _pBuf[i] = _pWire->read();
    }
  }
  
  return size;
}

uint8_t DFRobot_WiFi_IoT_Module_I2C::parameterReturn(uint8_t config, uint8_t config1, uint8_t *pBuf)
{
  uint8_t buffer[2]={0};uint8_t datalen = 0;
  uint32_t startingTime = millis();
  while(1){
    readReg(IOT_COMMAND_REGTSTER, &buffer, 2);
    if(buffer[0] == config && buffer[1] < 0xFE){
      datalen = buffer[1];
      uint8_t *data = (uint8_t *)malloc(sizeof(uint8_t)*datalen+10);
      if(data == NULL){
        Serial.print("memory allocation failed");
      }
      if(getData(READ_DATA_REGISTER, data, datalen) != datalen){
        DBG("READ WIFI_IOT_ERROR!!!!!!");
        free(data);
      }else{
        for(uint8_t i =0; i < datalen ; i++){
          pBuf[i] = data[i];
        }
        pBuf[datalen]= '\0';
        DBG("true");
        free(data);
        return 0;
      }
    }else if(buffer[0] == config1 && buffer[1] < 0xFE){
      return 2;
    }else if (buffer[0] == 0x15 && buffer[1] < 0xFE){
      uint16_t dataLen = buffer[1] << 8;
      readReg(IOT_COMMAND_REGTSTER, &buffer, 1);
      dataLen = dataLen | buffer[0];
      uint8_t *data = (uint8_t *)malloc(sizeof(uint8_t)* (dataLen+10));
      if(data == NULL){
        Serial.print("memory allocation failed");
      }
      if(getData(READ_DATA_REGISTER, data, dataLen) != dataLen){
        DBG("READ WIFI_IOT_ERROR!!!!!!");
        free(data);
      }else{
        for(uint8_t i =0; i < dataLen ; i++){
          pBuf[i] = data[i];
        }
        pBuf[datalen]= '\0';
        DBG("true");
        free(data);
        return 0;
      }
    }
    if((millis() - startingTime) > 15000){
      return 1;
    }
  }
  
  #if 0
  if(readReg(IOT_COMMAND_REGTSTER, &buffer, 2) != 2){
    DBG("READ WIFI_IOT_ERROR!!!!!!");
  }else{
    DBG(buffer[0]);
    if(buffer[0] == config && buffer[1] < 0xFE){
      
      uint8_t datalen = buffer[1];
      uint8_t *data = (uint8_t *)malloc(sizeof(uint8_t)*datalen+10);
      if(data == NULL){
        Serial.print("memory allocation failed");
      }
      if(getData(READ_DATA_REGISTER, data, datalen) != datalen){
        DBG("READ WIFI_IOT_ERROR!!!!!!");
        free(data);
      }else{
        for(uint8_t i =0; i < datalen ; i++){
          pBuf[i] = data[i];
        }
        pBuf[datalen]= '\0';
        DBG("true");
        free(data);
        return 0;
      }
    }else if(buffer[0] == config1){
      DBG("B");
      pBuf[0] = buffer[1];
      
    }else{
      return 1;
    }
  }
  #endif
  return 1;
}