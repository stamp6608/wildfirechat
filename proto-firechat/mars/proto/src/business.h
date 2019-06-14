//
//  business.h
//  proto
//
//  Created by WF Chat on 2017/11/4.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef business_hpp
#define business_hpp

#include <stdio.h>
#include <string>
#include <list>
#include "mars/stn/stn_logic.h"
#include "mars/stn/stn.h"
#include "mars/proto/src/Proto/pbbase.h"
#include "mars/proto/proto.h"




#define NOOP_CMDID 6
#define SIGNALKEEP_CMDID 243
#define PUSH_DATA_TASKID 0
#define MQTT_CONNECT_CMDID 10
#define MQTT_SEND_OUT_CMDID 11
#define MQTT_DISCONNECT_CMDID 12
#define MQTT_SUBSCRIBE_CMDID 13
#define MQTT_UNSUBSCRIBE_CMDID 14
#define MQTT_PUBACK_CMDID 15

#define UPLOAD_SEND_OUT_CMDID 20


namespace mars{
    namespace stn{

        typedef enum : int {
            MQTT_MSG_CONNECT = 1<<4,
            MQTT_MSG_CONNACK = 2<<4,
            MQTT_MSG_PUBLISH = 3<<4,
            MQTT_MSG_PUBACK = 4<<4,
            MQTT_MSG_PUBREC = 5<<4,
            MQTT_MSG_PUBREL = 6<<4,
            MQTT_MSG_PUBCOMP = 7<<4,
            MQTT_MSG_SUBSCRIBE = 8<<4,
            MQTT_MSG_SUBACK = 9<<4,
            MQTT_MSG_UNSUBSCRIBE = 10<<4,
            MQTT_MSG_UNSUBACK = 11<<4,
            MQTT_MSG_PINGREQ = 12<<4,
            MQTT_MSG_PINGRESP = 13<<4,
            MQTT_MSG_DISCONNECT = 14<<4
        } MQTT_MSG_TYPE;
        
        typedef enum : int32_t {
            ChannelType_ShortConn = 1,
            ChannelType_LongConn = 2,
            ChannelType_All = 3
        } ChannelType;

        



class MQTTGeneralCallback {
public:
    virtual void onSuccess() = 0;
    virtual void onFalure(int errorCode) = 0;
};

class MQTTPublishCallback {
public:
    virtual void onSuccess(const unsigned char* data, size_t len) = 0;
    virtual void onFalure(int errorCode) = 0;
};



class UploadTask : public Task {
public:
    UploadTask(const std::string &data, const std::string &token, int mediaType, UploadMediaCallback *callback);
public:
    std::string mData;
    std::string mToken;
    unsigned char mMediaType;
    UploadMediaCallback *mCallback;
    virtual ~UploadTask() {}
};


class MQTTTask : public Task {
protected:
    MQTTTask(MQTT_MSG_TYPE type);
public:
    const MQTT_MSG_TYPE type;
    std::string topic;
    virtual const std::string description() const;
    virtual ~MQTTTask() {}
};

class MQTTPublishTask : public MQTTTask {
public:
    MQTTPublishTask(MQTTPublishCallback *callback);
    PBBase *pbData;
    unsigned char* body;
    size_t length;
    MQTTPublishCallback *m_callback;
    virtual const std::string description() const;
    virtual ~MQTTPublishTask() ;
};

class MQTTSubscribeTask : public MQTTTask {
public:
    MQTTSubscribeTask(MQTTGeneralCallback *callback);
    MQTTGeneralCallback *m_callback;
    virtual ~MQTTSubscribeTask() {}
};

class MQTTUnsubscribeTask : public MQTTTask {
public:
    MQTTUnsubscribeTask(MQTTGeneralCallback *callback);
    MQTTGeneralCallback *m_callback;
    virtual ~MQTTUnsubscribeTask() {}
};

class MQTTPubAckTask : public MQTTTask {
public:
    MQTTPubAckTask(uint16_t messageId);
    virtual ~MQTTPubAckTask() {}
};

class MQTTDisconnectTask : public MQTTTask {
public:
    MQTTDisconnectTask();
    uint8_t flag;
    virtual ~MQTTDisconnectTask() {}
};

extern void PullFriend(int64_t head);
extern void PullFriendRequest(int64_t head);
extern std::string gUserId;
extern std::string gToken;
        
        
        class UserServerAddress {
        public:
            UserServerAddress():longLinkPort(0),shortLinkPort(0),updateDt(0) {}
            std::string host;
            int longLinkPort;
            int shortLinkPort;
            int64_t updateDt;
            virtual ~UserServerAddress(){}
        };
        
        extern void RequestRoute(bool force);
    }}
#endif /* business_h */
