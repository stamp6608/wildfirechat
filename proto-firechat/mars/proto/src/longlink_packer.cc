// Tencent is pleased to support the open source community by making Mars available.
// Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

// Licensed under the MIT License (the "License"); you may not use this file except in 
// compliance with the License. You may obtain a copy of the License at
// http://opensource.org/licenses/MIT

// Unless required by applicable law or agreed to in writing, software distributed under the License is
// distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied. See the License for the specific language governing permissions and
// limitations under the License.


/*
 * longlink_packer.cc
 *
 *  Created on: 2012-7-18
 *      Author: yerungui, caoshaokun
 */

#include "mars/stn/proto/longlink_packer.h"

//#include <arpa/inet.h>

#include "mars/comm/xlogger/xlogger.h"
#include "mars/comm/autobuffer.h"
#include "mars/stn/stn.h"
#include "mars/stn/stn_logic.h"
#include "proto/src/libemqtt.h"
#include "proto/src/business.h"


/*

MQTT_MSG_CONNECT       -->    MQTT_CONNECT_CMDID
MQTT_MSG_CONNACK       <--    MQTT_CONNECT_CMDID
MQTT_MSG_PUBLISH       -->    MQTT_SEND_OUT_CMDID
MQTT_MSG_PUBLISH       <--    PUSH_DATA_TASKID
MQTT_MSG_PUBACK        -->    MQTT_SEND_OUT_CMDID
MQTT_MSG_PUBACK        <--    PUSH_DATA_TASKID
MQTT_MSG_PINGREQ       -->    NOOP_CMDID
MQTT_MSG_PINGRESP      <--    NOOP_CMDID
MQTT_MSG_DISCONNECT    -->    MQTT_DISCONNECT_CMDID
 
 MQTT_MSG_PUBREC        5<<4
 MQTT_MSG_PUBREL        6<<4
 MQTT_MSG_PUBCOMP       7<<4
 MQTT_MSG_SUBSCRIBE     8<<4
 MQTT_MSG_SUBACK        9<<4
 MQTT_MSG_UNSUBSCRIBE  10<<4
 MQTT_MSG_UNSUBACK     11<<4
 
 */

extern unsigned char * decrypt_data(const unsigned char* data, unsigned int data_length, unsigned int *output_length, bool rootKey, bool checkTime);

namespace mars {
namespace stn {
longlink_tracker* (*longlink_tracker::Create)()
= []() {
    return new longlink_tracker;
};

void (*longlink_pack)(uint32_t _cmdid, uint32_t _seq, const AutoBuffer& _body, const AutoBuffer& _extension, AutoBuffer& _packed, longlink_tracker* _tracker)
= [](uint32_t _cmdid, uint32_t _seq, const AutoBuffer& _body, const AutoBuffer& _extension, AutoBuffer& _packed, longlink_tracker* _tracker) {
  switch(_cmdid) {
    case NOOP_CMDID:
      mqtt_ping(_packed);
      break;
    case SIGNALKEEP_CMDID:
      break;
    case PUSH_DATA_TASKID:
      break;
    case MQTT_CONNECT_CMDID:
      mqtt_connect(_packed);
      break;
    case MQTT_SEND_OUT_CMDID:
          
      mqtt_publish_with_qos((char *)_body.Ptr(), (unsigned char *)_extension.Ptr(), _extension.Length(), 1, 1, 1, _seq, _packed);
      break;
    case MQTT_SUBSCRIBE_CMDID:
      mqtt_subscribe((char *)_body.Ptr(), _seq, _packed);
      break;
    case MQTT_UNSUBSCRIBE_CMDID:
      mqtt_unsubscribe((char *)_body.Ptr(), _seq, _packed);
      break;
    case MQTT_DISCONNECT_CMDID:
      mqtt_disconnect(_packed, *((uint8_t *)_body.Ptr()));
      break;
    case MQTT_PUBACK_CMDID:
      mqtt_puback(_seq, _packed);
      break;
  }
  _packed.Seek(0, AutoBuffer::ESeekStart);
};
    
int (*longlink_unpack)(const AutoBuffer& _packed, uint32_t& _cmdid, uint32_t& _seq, size_t& _package_len, AutoBuffer& _body, AutoBuffer& _extension, longlink_tracker* _tracker)
    = [](const AutoBuffer& _packed, uint32_t& _cmdid, uint32_t& _seq, size_t& _package_len, AutoBuffer& _body, AutoBuffer& _extension, longlink_tracker* _tracker) {

    size_t _body_len = 0;
    
    if (_packed.Length() < 2) {
        return LONGLINK_UNPACK_CONTINUE;
    }
    
    unsigned char *p = (unsigned char *)_packed.Ptr();
    p++;
    if (*p & 0x80) {
        if (_packed.Length() < 4) {
            return LONGLINK_UNPACK_CONTINUE;
        }
    }
    
    const unsigned char *data = ( unsigned char *)_packed.Ptr();
    int packLen = mqtt_parse_rem_len(data);
    int remainHeaderBytes = mqtt_num_rem_len_bytes(data);
    
    if ((size_t)(packLen + 1 + remainHeaderBytes) > _packed.Length()) {
        return LONGLINK_UNPACK_CONTINUE;
    }
    
    _package_len = packLen + 1 + remainHeaderBytes;
    _body_len = _packed.Length();
    
    if (_package_len > 4* 1024*1024) { return LONGLINK_UNPACK_FALSE; }
    if (_package_len > _packed.Length()) { return LONGLINK_UNPACK_CONTINUE; }
    
    
    switch (MQTTParseMessageType(( unsigned char *)_packed.Ptr())) {
        case MQTT_MSG_CONNACK:
            _cmdid = MQTT_CONNECT_CMDID;
            _seq = Task::kLongLinkIdentifyCheckerTaskID;
            _body_len = _package_len - 1 - remainHeaderBytes;
            break;
            
        case MQTT_MSG_PUBLISH:
        {
            _cmdid = PUSH_DATA_TASKID;
            _seq = 0;
            _body_len = _package_len -  1 - remainHeaderBytes;
            uint8_t stackBuf[8*1024];
            uint8_t *buffer;
            
            if (_body_len < 8* 1024) {
                buffer = stackBuf;
            } else {
                buffer = new uint8_t[_body_len]();
            }
            
            uint16_t length = mqtt_parse_pub_topic((const uint8_t*)_packed.Ptr(), buffer);
            _body.AllocWrite(length);
            _body.Write(buffer, length);
            
            length = mqtt_parse_publish_msg((const uint8_t*)_packed.Ptr(), buffer);
            _extension.AllocWrite(length);
            _extension.Write(buffer, length);
            
            if (_body_len > 8* 1024) {
                delete [] buffer;
            }
            
            int messageId = mqtt_parse_msg_id((const uint8_t*)_packed.Ptr());
            int qos = MQTTParseMessageQos((const uint8_t*)_packed.Ptr());
            if (qos == 0) {
                //no need response
            } else if (qos == 1) {
                MQTTPubAckTask *ackTask = new MQTTPubAckTask(messageId);
                StartTask(*ackTask);
            } else if (qos == 2) {
                //not support
            }
            return LONGLINK_UNPACK_OK;
        }
            break;
        case MQTT_MSG_PUBACK: {
            _cmdid = MQTT_SEND_OUT_CMDID;
            _seq = mqtt_parse_msg_id((const uint8_t*)_packed.Ptr());
            _body_len = _package_len - 1 - remainHeaderBytes - 2;
            
            _body.Write((const unsigned char*)_packed.Ptr() + (_package_len-_body_len), 1);
            
            if(_body_len > 1) {
                unsigned int dataLen = 0;
                unsigned char* pdata = (unsigned char* )decrypt_data((const unsigned char*)_packed.Ptr() + (_package_len-_body_len) + 1,(unsigned int) _body_len-1, &dataLen, false, true);
                _body.Write(pdata, dataLen);
                free(pdata);
            }
            return LONGLINK_UNPACK_OK;
            break;
        }
            
        case MQTT_MSG_PUBREC:
        case MQTT_MSG_PUBREL:
        case MQTT_MSG_PUBCOMP:
            break;
        case MQTT_MSG_SUBACK:
            _cmdid = MQTT_SUBSCRIBE_CMDID;
            _seq = mqtt_parse_msg_id((const uint8_t*)_packed.Ptr());
            _body_len = _package_len - 1 - remainHeaderBytes - 2;
            break;
        case MQTT_MSG_UNSUBACK:
            _cmdid = MQTT_UNSUBSCRIBE_CMDID;
            _seq = mqtt_parse_msg_id((const uint8_t*)_packed.Ptr());
            _body_len = _package_len - 1 - remainHeaderBytes - 2;
            break;
            
        case MQTT_MSG_PINGRESP:
            _cmdid = NOOP_CMDID;
            _seq = Task::kNoopTaskID;
            break;
            
        case MQTT_MSG_SUBSCRIBE:
        case MQTT_MSG_UNSUBSCRIBE:
        case MQTT_MSG_PINGREQ:
        case MQTT_MSG_CONNECT:
        case MQTT_MSG_DISCONNECT:
            //can not receive
            break;
            
        default:
            break;
    }
    
    _body.Write(AutoBuffer::ESeekStart, (const unsigned char*)_packed.Ptr() + (_package_len-_body_len), _body_len);
    
    return LONGLINK_UNPACK_OK;

};


uint32_t (*longlink_noop_cmdid)()
= []() -> uint32_t {
    return NOOP_CMDID;
};

bool  (*longlink_noop_isresp)(uint32_t _taskid, uint32_t _cmdid, uint32_t _recv_seq, const AutoBuffer& _body, const AutoBuffer& _extend)
= [](uint32_t _taskid, uint32_t _cmdid, uint32_t _recv_seq, const AutoBuffer& _body, const AutoBuffer& _extend) {
    return Task::kNoopTaskID == _taskid && NOOP_CMDID == _cmdid;
};

uint32_t (*signal_keep_cmdid)()
= []() -> uint32_t {
    return SIGNALKEEP_CMDID;
};

void (*longlink_noop_req_body)(AutoBuffer& _body, AutoBuffer& _extend)
= [](AutoBuffer& _body, AutoBuffer& _extend) {
    
};
    
void (*longlink_noop_resp_body)(const AutoBuffer& _body, const AutoBuffer& _extend)
= [](const AutoBuffer& _body, const AutoBuffer& _extend) {
    
};

uint32_t (*longlink_noop_interval)()
= []() -> uint32_t {
	return 0;
};

bool (*longlink_complexconnect_need_verify)()
= []() {
    return false;
};

bool (*longlink_ispush)(uint32_t _cmdid, uint32_t _taskid, const AutoBuffer& _body, const AutoBuffer& _extend)
= [](uint32_t _cmdid, uint32_t _taskid, const AutoBuffer& _body, const AutoBuffer& _extend) {
    return PUSH_DATA_TASKID == _taskid;
};
    
bool (*longlink_identify_isresp)(uint32_t _sent_seq, uint32_t _cmdid, uint32_t _recv_seq, const AutoBuffer& _body, const AutoBuffer& _extend)
= [](uint32_t _sent_seq, uint32_t _cmdid, uint32_t _recv_seq, const AutoBuffer& _body, const AutoBuffer& _extend) {
    return _sent_seq == _recv_seq && PUSH_DATA_TASKID != _sent_seq;
};

}
}
