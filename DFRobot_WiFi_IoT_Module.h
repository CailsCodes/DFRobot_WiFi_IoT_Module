/*!
 * @file DFRobot_WiFi_IoT_Module.h
 * @brief This is an library of Wifi IoT module from DFRobot 
 * @n Use the WiFi IOT module with this library to easily access IoT platforms such as IFTTT, ThingSpeak, EasyIoT, SIOT
 * @copyright    Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence      The MIT License (MIT)
 * @author       [Tang](jie.tang@dfrobot.com)
 * @version      V1.0
 * @eGDAte       2020-06-17
 * @get          from https://www.dfrobot.com
 * @url          https://github.com/DFRobot/DFRobot_WiFi_IOT_Module
 */

#ifndef _DFRobot_WiFi_IoT_Module_H
#define _DFRobot_WiFi_IoT_Module_H

//#include <Arduino.h>
#include <Wire.h>


#define    WIFI_IOT_ERROR         5
#define    WIFI_IOT_OK            0

#define ENABLE_DBG
#ifdef ENABLE_DBG
#define DBG(...) {Serial.print("[");Serial.print(__FUNCTION__); Serial.print("(): "); Serial.print(__LINE__); Serial.print(" ] "); Serial.println(__VA_ARGS__);}
#else
#define DBG(...)
#endif

class DFRobot_WiFi_IoT_Module
{
public:
  typedef void (*callback)(const char*topic, const char*message);
  /**
   * @brief Init communication port  
   */
  virtual uint8_t begin(void) = 0;
  /**
   * @brief Configure WiFi connection network
   * @param ssid WiFi name
   * @param pwd Wifi password
   * @return 0: WiFi connected, 1：WiFi connection error, 2: Connecting to WiFi
   */
  virtual uint8_t connectWifi(char *ssid, char *pwd) = 0;
  /**
   * @brief   MQTT configuration
   * @param server  Server address
   * @param port  IOT port number
   * @param id    IOT ID name
   * @param pwd   IOT access password
   * @return  Mqtt connection status
   */ 
  virtual uint8_t MQTTBegin(char *server, char *port, char *productID, char *pwd, char* deviceID) = 0;
  /**
   * @brief Title Subscription, up to 5
   * @param topic  Subscription channel 
   * @return 0 subscription succeeded, 1 subscription failed, 2 Reached subscription upper-limit, 3 subscription started, null None, 5 no subscription object
   */
  virtual uint8_t subscribe(char *topic = NULL) = 0;
  /**
   * @brief  Send message to title
   * @param data Data to be sent  
   * @param topic  Subscription channel 
   * @return 0：succeeded, -1: failed
   */
  virtual uint8_t publish(char *topic, String data) = 0;
  /**
   * @brief Get version number
   * @return Version information
   */
  virtual String getVersion() = 0;
  /**
   * @brief Loop read status
   */
  virtual uint8_t loop() = 0;
  /**
   * @brief Set callback function
   */
  // virtual void setCallBack(callback call) = 0;

  uint8_t _wifiState       = 254;
  TwoWire *_pWire;
  uint8_t _address;
  callback _callback       = NULL;

};

class DFRobot_WiFi_IoT_Module_I2C:public DFRobot_WiFi_IoT_Module
{
public:
  #define    IOT_COMMAND_REGTSTER  0X1E           //Command register

  /* Register command*/ 

  #define    IOT_RUN_COMMAND        0X02           //WIFI_IOT running command
  #define    IOT_READ_COMMAND       0X00           //WIFI_IOT read command
  #define    IOT_SET_COMMAND        0X01           //WIFI_IOT setup command
  #define    IOT_TYPE_REGISTER      0X1F           //Parameter type register

  /* Parameter type setting */ 

  #define    SET_WIFI_NAME          0X01           
  #define    SET_WIFI_PASSWORD      0X02
  #define    SET_MQTT_SERVER        0X03
  #define    SET_MQTT_PORT          0X04
  #define    SET_MQTT_ID            0X05
  #define    SET_MQTT_PASSWORD      0X06
  #define    SET_HTTP_ID            0X07
  #define    SET_HTTP_PORT          0X08
  #define    SET_MQTT_DEVICEID      0x09

  #define    SEND_PING              0X01
  #define    CONNECT_WIFI           0X02
  #define    RECONNECT_WIFI         0X03
  #define    BREAK_WIFI_CON         0X04
  #define    CONNECT_MQTT           0X05
  #define    SUBSCRIBE_TOPIC0       0X06
  #define    SUBSCRIBE_TOPIC1       0X07
  #define    SUBSCRIBE_TOPIC2       0X08
  #define    SUBSCRIBE_TOPIC3       0X09
  #define    SUBSCRIBE_TOPIC4       0X0A
  #define    SEND_TOPIC0            0X0B
  #define    SEND_TOPIC1            0X0C
  #define    SEND_TOPIC2            0X0D
  #define    SEND_TOPIC3            0X0E
  #define    SEND_TOPIC4            0X0F
  #define    HTTP_GET_URL           0X10
  #define    HTTP_POST_URL_CON      0X11
  #define    HTTP_PUT_URL_CON       0X12
  #define    QUERY_VERSION          0X13
  #define    CONNECT_MQTT_ONENET    0x14

  #define    READ_COMMAND           0X06           //Read register

  #define    IOT_LEN_REGISTER       0X20           //Register for storing parameter length 

  /* The parameter type of the returned value */ 

  #define    NONE                   0X00
  #define    PING_STATUE            0X01
  #define    WIFI_STATUE_CON        0X02
  #define    IP_ADDRESS             0X03
  #define    MQTT_STATUE_CON        0X04
  #define    SUBSCRIBE_STATUE       0X05
  #define    SUBSCRIBE_TOPIC4       0X0A
  #define    HTTP_NORMAL_RETURN     0X10
  #define    HTTP_ERROR_RETURN      0X11
  #define    GET_VERSION            0X12
  #define    GET_MQTTSEND_STATUE    0X13

  /* Data register */ 

  #define    READ_LEN_REGISTER      0X21
  #define    READ_DATA_REGISTER     0X22

  #define    PING_SUCCESS           0X01
  #define    PING_ERROR             0X00

  #define    WIFI_NO_CON            0X00
  #define    WIFI_RUN_CON           0X02
  #define    WIFI_SUCCESS_CON       0X03

  #define    MQTT_START             0X00
  #define    MQTT_SUCCESS_CON       0X01
  #define    MQTT_ERROR_CON         0X02

  #define    TAKE_START             0X00
  #define    TAKE_SUCCESS           0X01
  #define    TAKE_UPPER_LIMIT       0X02
  #define    TAKE_ERROR             0X03

  #define    RECV_TOPIC0            0X06
  #define    RECV_TOPIC1            0X07
  #define    RECV_TOPIC2            0X08
  #define    RECV_TOPIC3            0X09
  #define    RECV_TOPIC4            0X0A

  #define    RETURN_NONE            0X0A


  #define    TOPIC0                 0
  #define    TOPIC1                 1
  #define    TOPIC2                 2
  #define    TOPIC3                 3
  #define    TOPIC4                 4

  #define    FULL_SUBSCRIPTINO      6
  #define    REPEAT_SUBSCRIPTION    7

  #define   MQTT_SEND_SUCCESS      0X01
  #define   MQTT_SEND_ERROR        0x00 

  DFRobot_WiFi_IoT_Module_I2C(TwoWire *pWire = &Wire, uint8_t address = 0x16)
  {
    _pWire   = pWire;
    _address = address; 
  };
  /**
   * @brief   Initial I2C
   */
  uint8_t begin(void);
  /**
   * @brief Configure WiFi Connection network
   * @param ssid  WiFi name
   * @param pwd Wifi password
   * @return 0: Connected to WiFi, 1：WiFi connection error, 2: Connecting to WiFi
   */
  uint8_t connectWifi(char *ssid, char *pwd);
  /**
   * @brief MQTT configuration
   * @param server Server address
   * @param port IOT port number
   * @param id   IOT ID name
   * @param pwd  IOT access password
   * @return Mqtt connection status
   */ 
  uint8_t MQTTBegin(char *server = NULL, char *port = NULL, char *productID  = NULL, char *pwd = NULL, char* deviceID = NULL);
  /**
   * @brief Title Subscription, up to 5
   * @param topic  Subscription channel 
   * @return 0 subscription succeeded, 1 subscription failed, 2 Reached subscription upper-limit, 3 subscription started, null None, 5 no subscription object
   */
  uint8_t subscribe(char *topic = NULL);
  /**
   * @brief Send message to title
   * @param data Data to be sent
   * @param topic    Subscription channel 
   * @return 0：succeeded, -1: failed
   */
  uint8_t publish(char *topic, String data);
  /**
   * @brief  Send any message to the title
   * @param data Data to be sent  
   * @param topic  Subscription channel 
   * @param len Data length
   * @return 0：succeeded, -1: failed
   */
  uint8_t publish(char *topic, uint8_t *data, uint16_t len);
  /**
   * @brief Get version number
   * @return Version information
   */
  String getVersion();
  /**
   * @brief  Loop read status
   */
  uint8_t loop();
  /**
   * @brief Set callback function
   */
  void setCallBack(callback call);
  
private:
  void clearBuffer(void);
  /**
   * @brief  Write register command
   */
  void connection(uint8_t command);
  /**
   * @brief Write register commands with data.
   */
  void manageFunction(uint8_t command, uint8_t config, String data);
  /**
   *@brief HTTP return data
   */
  uint8_t parameterReturn(uint8_t config, uint8_t config1, uint8_t *pBuf);
  /**
   * @brief Get WiFi connection status
   */
  uint8_t readWifiState();
  /**
   * @brief Get MQTT connection status
   */
  uint8_t getMQTTState();
  /**
   * @brief Get MQTT sendmessage status
   */
  uint8_t getMQTTSendState();
  /**
   * @brief  Get topic subscription status
   */
  uint8_t getTopicState();
  /**
   * @brief  Reconnect WiFi 
   */
  void reconnectWifi();
  /**
   * @brief   Disconnect WiFi
   * @return  True disconnected successfully
   */
  bool disconnectWifi();
  /**
   * @brief   Get WiFi status
   * @return   WiFi status information
   */
  uint8_t getWifiState();
  /**
   * @brief   Write register value through IIC bus
   * @param reg  Register address 8bits
   * @param pBuf Storage cache to write data in
   * @param size The length of data to be written
   */
  void writeReg(uint8_t reg, void *pBuf, size_t size);
  /**
   *@brief Read register value through IIC bus
   * @param reg  Register address 8bits
   * @param pBuf Read data storage cache
   * @param size Read the length of data
   * @return Return the read length, 0 indicates it failed to read 
   */
  uint8_t readReg(uint8_t reg, void *pBuf, size_t size);
  /**
   * @brief   Read register value through IIC bus
   * @param reg     Register address 8bits
   * @param pBuf   Read data storage cache
   * @param size    Read the length of data
   * @return  Return the read length, 0 indicates it failed to read
   */
  uint8_t getData(uint8_t reg, void *pBuf, size_t size);

};

//The location of the system message protocol field
class systemProtocol
{
public:
  static const uint8_t systemType    = 0;
  static const uint8_t systemCode    = 1;
  static const uint8_t systemMessage = 2;
};

//Location of WiFi protocol fields
class wifiProtocol
{
public:
  static const uint8_t wifiType    = 0;
  static const uint8_t wifiCode    = 1;
  static const uint8_t wifiMessage = 2;
};

//Location of HTTP protocol fields
class httpProtocol
{
public:
  static const uint8_t httpType    = 0;
  static const uint8_t httpCode    = 1;
  static const uint8_t httpMessage = 2;
};

//The position of mqtt protocol fields
class mqttProtocol
{
public:
  static const uint8_t mqttType      = 0;
  static const uint8_t mqttMethod    = 1;
  static const uint8_t mqttFunction  = 2;
  static const uint8_t mqttCode      = 3;
  static const uint8_t mqttMessage   = 4;
  static const uint8_t mqttTopic     = 3;
};

#endif