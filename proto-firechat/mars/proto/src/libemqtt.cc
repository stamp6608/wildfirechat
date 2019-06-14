/*
 * This file is part of libemqtt.
 *
 * libemqtt is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libemqtt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libemqtt.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 *
 * Created by Filipe Varela on 09/10/16.
 * Copyright 2009 Caixa Mágica Software. All rights reserved.
 *
 * Fork developed by Vicente Ruiz Rodríguez
 * Copyright 2012 Vicente Ruiz Rodríguez <vruiz2.0@gmail.com>. All rights reserved.
 *
 */

#include <string.h>
#include "mars/stn/stn.h"
#include "libemqtt.h"
#include "proto/src/business.h"
#include "mars/app/app.h"
#include "aes_crypt.h"

#define MQTT_DUP_FLAG     1<<3
#define MQTT_QOS0_FLAG    0<<1
#define MQTT_QOS1_FLAG    1<<1
#define MQTT_QOS2_FLAG    2<<1

#define MQTT_RETAIN_FLAG  1

#define MQTT_CLEAN_SESSION  1<<1
#define MQTT_WILL_FLAG      1<<2
#define MQTT_WILL_RETAIN    1<<5
#define MQTT_USERNAME_FLAG  1<<7
#define MQTT_PASSWORD_FLAG  1<<6

static mqtt_broker_handle_t g_broker;
static mqtt_broker_handle_t *broker = &g_broker;
extern unsigned char * encrypt_data(const unsigned char* data, unsigned int data_length, unsigned int *output_length, bool rootKey);
extern unsigned char * decrypt_data(const unsigned char* data, unsigned int data_length, unsigned int *output_length, bool rootKey, bool checkTime);

uint8_t mqtt_num_rem_len_bytes(const uint8_t* buf) {
	uint8_t num_bytes = 1;
	
	//printf("mqtt_num_rem_len_bytes\n");
	
	if ((buf[1] & 0x80) == 0x80) {
		num_bytes++;
		if ((buf[2] & 0x80) == 0x80) {
			num_bytes ++;
			if ((buf[3] & 0x80) == 0x80) {
				num_bytes ++;
			}
		}
	}
	return num_bytes;
}

uint32_t mqtt_parse_rem_len(const uint8_t* buf) {
	uint32_t multiplier = 1;
	uint32_t value = 0;
	uint8_t digit;
	
	//printf("mqtt_parse_rem_len\n");
	
	buf++;	// skip "flags" byte in fixed header

	do {
		digit = *buf;
		value += (digit & 127) * multiplier;
		multiplier *= 128;
		buf++;
	} while ((digit & 128) != 0);

	return value;
}

uint8_t mqtt_parse_msg_id(const uint8_t* buf) {
	uint8_t type = MQTTParseMessageType(buf);
	uint8_t qos = MQTTParseMessageQos(buf);
	uint16_t id = 0;
	
	//printf("mqtt_parse_msg_id\n");
	
	if(type >= mars::stn::MQTT_MSG_PUBLISH && type <= mars::stn::MQTT_MSG_UNSUBACK) {
		if(type == mars::stn::MQTT_MSG_PUBLISH) {
			if(qos != 0) {
				// fixed header length + Topic (UTF encoded)
				// = 1 for "flags" byte + rlb for length bytes + topic size
				uint8_t rlb = mqtt_num_rem_len_bytes(buf);
				uint8_t offset = *(buf+1+rlb)<<8;	// topic UTF MSB
				offset |= *(buf+1+rlb+1);			// topic UTF LSB
				offset += (1+rlb+2);					// fixed header + topic size
				id = *(buf+offset)<<8;				// id MSB
				id |= *(buf+offset+1);				// id LSB
			}
		} else {
			// fixed header length
			// 1 for "flags" byte + rlb for length bytes
			uint8_t rlb = mqtt_num_rem_len_bytes(buf);
			id = *(buf+1+rlb)<<8;	// id MSB
			id |= *(buf+1+rlb+1);	// id LSB
		}
	}
	return id;
}

uint16_t mqtt_parse_pub_topic(const uint8_t* buf, uint8_t* topic) {
	const uint8_t* ptr;
	uint16_t topic_len = mqtt_parse_pub_topic_ptr(buf, &ptr);
	
	//printf("mqtt_parse_pub_topic\n");
	
	if(topic_len != 0 && ptr != NULL) {
		memcpy(topic, ptr, topic_len);
	}
	
	return topic_len;
}

uint16_t mqtt_parse_pub_topic_ptr(const uint8_t* buf, const uint8_t **topic_ptr) {
	uint16_t len = 0;
	
	//printf("mqtt_parse_pub_topic_ptr\n");

	if(MQTTParseMessageType(buf) == mars::stn::MQTT_MSG_PUBLISH) {
		// fixed header length = 1 for "flags" byte + rlb for length bytes
		uint8_t rlb = mqtt_num_rem_len_bytes(buf);
		len = *(buf+1+rlb)<<8;	// MSB of topic UTF
		len |= *(buf+1+rlb+1);	// LSB of topic UTF
		// start of topic = add 1 for "flags", rlb for remaining length, 2 for UTF
		*topic_ptr = (buf + (1+rlb+2));
	} else {
		*topic_ptr = NULL;
	}
	return len;
}

uint16_t mqtt_parse_publish_msg(const uint8_t* buf, uint8_t* msg) {
	const uint8_t* ptr;
	
	//printf("mqtt_parse_publish_msg\n");
	
	uint16_t msg_len = mqtt_parse_pub_msg_ptr(buf, &ptr);
	
	if(msg_len != 0 && ptr != NULL) {
        memcpy(msg, ptr, msg_len);
	}
	
	return msg_len;
}

uint16_t mqtt_parse_pub_msg_ptr(const uint8_t* buf, const uint8_t **msg_ptr) {
	uint16_t len = 0;
	
	//printf("mqtt_parse_pub_msg_ptr\n");
	
	if(MQTTParseMessageType(buf) == mars::stn::MQTT_MSG_PUBLISH) {
		// message starts at
		// fixed header length + Topic (UTF encoded) + msg id (if QoS>0)
		uint8_t rlb = mqtt_num_rem_len_bytes(buf);
		uint8_t offset = (*(buf+1+rlb))<<8;	// topic UTF MSB
		offset |= *(buf+1+rlb+1);			// topic UTF LSB
		offset += (1+rlb+2);				// fixed header + topic size

		if(MQTTParseMessageQos(buf)) {
			offset += 2;					// add two bytes of msg id
		}

		*msg_ptr = (buf + offset);
				
		// offset is now pointing to start of message
		// length of the message is remaining length - variable header
		// variable header is offset - fixed header
		// fixed header is 1 + rlb
		// so, lom = remlen - (offset - (1+rlb))
      	len = mqtt_parse_rem_len(buf) - (offset-(rlb+1));
	} else {
		*msg_ptr = NULL;
	}
	return len;
}

void mqtt_init(const char* clientid) {
	// Connection options
	broker->alive = 300; // 300 seconds = 5 minutes
	// Client options
	memset(broker->clientid, 0, sizeof(broker->clientid));
	memset(broker->username, 0, sizeof(broker->username));
	memset(broker->password, 0, sizeof(broker->password));
	if(clientid) {
		strncpy(broker->clientid, clientid, sizeof(broker->clientid));
	} else {
		strcpy(broker->clientid, "emqtt");
	}
	// Will topic
	broker->clean_session = 1;
}

void mqtt_init_auth(const char* username, const char* password) {
	if(username && username[0] != '\0')
		strncpy(broker->username, username, sizeof(broker->username)-1);
	if(password && password[0] != '\0')
		strncpy(broker->password, password, sizeof(broker->password)-1);
}

void mqtt_set_alive(uint16_t alive) {
	broker->alive = alive;
}

int mqtt_connect(AutoBuffer& _packed)
{
	uint8_t flags = 0x00;

	uint16_t clientidlen = strlen(broker->clientid);
	uint16_t usernamelen = strlen(broker->username);
    
    unsigned int pwdLen = 0;
    char *pPwd = (char *)encrypt_data((unsigned char*)broker->password, (unsigned int)strlen(broker->password), &pwdLen, false);
  
	uint16_t passwordlen = pwdLen;
	uint16_t payload_len = clientidlen + 2;

	// Preparing the flags
	if(usernamelen) {
		payload_len += usernamelen + 2;
		flags |= MQTT_USERNAME_FLAG;
	}
	if(passwordlen) {
		payload_len += passwordlen + 2;
		flags |= MQTT_PASSWORD_FLAG;
	}
	if(broker->clean_session) {
		flags |= MQTT_CLEAN_SESSION;
	}

	// Variable header
	uint8_t var_header[] = {
		0x00,0x04,0x4d,0x51,'T','T', // Protocol name: MQTT
		0x04, // Protocol version
		flags, // Connect flags
		(uint8_t)(broker->alive>>8),
    (uint8_t)(broker->alive&0xFF), // Keep alive
	};


   	// Fixed header
    uint8_t fixedHeaderSize = 2;    // Default size = one byte Message Type + one byte Remaining Length
    uint8_t remainLen = sizeof(var_header)+payload_len;
    if (remainLen > 127) {
        fixedHeaderSize++;          // add an additional byte for Remaining Length
    }
    uint8_t *fixed_header = (uint8_t *)malloc(sizeof(uint8_t)*fixedHeaderSize);
    
    // Message Type
    fixed_header[0] = mars::stn::MQTT_MSG_CONNECT;

    // Remaining Length
    if (remainLen <= 127) {
        fixed_header[1] = remainLen;
    } else {
        // first byte is remainder (mod) of 128, then set the MSB to indicate more bytes
        fixed_header[1] = remainLen % 128;
        fixed_header[1] = fixed_header[1] | 0x80;
        // second byte is number of 128s
        fixed_header[2] = remainLen / 128;
    }

	uint16_t offset = 0;
    size_t packetSize = sizeof(uint8_t)*fixedHeaderSize+sizeof(var_header)+payload_len;
	uint8_t *packet = (uint8_t *)malloc(sizeof(uint8_t)*packetSize);
	memset(packet, 0, sizeof(uint8_t)*packetSize);
	memcpy(packet, fixed_header, sizeof(uint8_t)*fixedHeaderSize);
	offset += (sizeof(uint8_t)*fixedHeaderSize);
	memcpy(packet+offset, var_header, sizeof(var_header));
	offset += sizeof(var_header);
	// Client ID - UTF encoded
	packet[offset++] = clientidlen>>8;
	packet[offset++] = clientidlen&0xFF;
	memcpy(packet+offset, broker->clientid, clientidlen);
	offset += clientidlen;

	if(usernamelen) {
		// Username - UTF encoded
		packet[offset++] = usernamelen>>8;
		packet[offset++] = usernamelen&0xFF;
		memcpy(packet+offset, broker->username, usernamelen);
		offset += usernamelen;
	}

	if(passwordlen) {
		// Password - UTF encoded
		packet[offset++] = passwordlen>>8;
		packet[offset++] = passwordlen&0xFF;
		memcpy(packet+offset, pPwd, passwordlen);
		offset += passwordlen;
	}
  
    if (passwordlen) {
        free(pPwd);
        pPwd = NULL;
        passwordlen = 0;
    }

  _packed.AllocWrite(sizeof(uint8_t)*packetSize);
  _packed.Write(packet, sizeof(uint8_t)*packetSize);
    
    free(fixed_header);
    fixed_header = NULL;
    free(packet);
    packet = NULL;
    
	return 1;
}

int mqtt_disconnect(AutoBuffer& _packed, uint8_t flag) {
    uint8_t headByte = mars::stn::MQTT_MSG_DISCONNECT;
    headByte |= flag;
	uint8_t packet[] = {
        headByte, // Message Type, DUP flag, QoS level, Retain
        0x00 // Remaining length
	};

  _packed.AllocWrite(sizeof(packet));
  _packed.Write(packet, sizeof(packet));

	return 1;
}

int mqtt_ping(AutoBuffer& _packed) {
	uint8_t packet[] = {
		mars::stn::MQTT_MSG_PINGREQ, // Message Type, DUP flag, QoS level, Retain
		0x00 // Remaining length
	};

  _packed.AllocWrite(sizeof(packet));
  _packed.Write(packet, sizeof(packet));

	return 1;
}

int mqtt_publish(const char* topic, const unsigned char* msg, size_t msglen, uint8_t retain, AutoBuffer& _packed) {
	return mqtt_publish_with_qos(topic, msg, msglen, 0, retain, 0, 0, _packed);
}

int mqtt_publish_with_qos(const char* topic, const unsigned char* msg, size_t msglen, uint8_t dup, uint8_t retain, uint8_t qos, uint16_t message_id, AutoBuffer& _packed) {
    
    unsigned int tmpLen = 0;
    unsigned char *ptmp = encrypt_data(msg, (unsigned int)msglen, &tmpLen, false);
    msg = ptmp;
    msglen = tmpLen;
    
	uint16_t topiclen = strlen(topic);

	uint8_t qos_flag = MQTT_QOS0_FLAG;
	uint8_t qos_size = 0; // No QoS included
	if(qos == 1) {
		qos_size = 2; // 2 bytes for QoS
		qos_flag = MQTT_QOS1_FLAG;
	}
	else if(qos == 2) {
		qos_size = 2; // 2 bytes for QoS
		qos_flag = MQTT_QOS2_FLAG;
	}

	// Variable header
    size_t varHeaderSize = topiclen+2+qos_size;
	uint8_t *var_header = (uint8_t *)malloc(sizeof(uint8_t)*varHeaderSize); // Topic size (2 bytes), utf-encoded topic
	memset(var_header, 0, sizeof(uint8_t)*varHeaderSize);
	var_header[0] = topiclen>>8;
	var_header[1] = topiclen&0xFF;
	memcpy(var_header+2, topic, topiclen);
	if(qos_size) {
		var_header[topiclen+2] = message_id>>8;
		var_header[topiclen+3] = message_id&0xFF;
	}

	// Fixed header
	// the remaining length is one byte for messages up to 127 bytes, then two bytes after that
	// actually, it can be up to 4 bytes but I'm making the assumption the embedded device will only
	// need up to two bytes of length (handles up to 16,383 (almost 16k) sized message)
	uint8_t fixedHeaderSize = 2;    // Default size = one byte Message Type + one byte Remaining Length
	uint16_t remainLen = sizeof(uint8_t)*varHeaderSize+msglen;
	if (remainLen > 127) {
		fixedHeaderSize++;          // add an additional byte for Remaining Length
	}
    if (remainLen > 16383) {
        fixedHeaderSize++;
    }
    uint8_t *fixed_header = (uint8_t *)malloc(sizeof(uint8_t) * fixedHeaderSize);
    
   // Message Type, DUP flag, QoS level, Retain
   fixed_header[0] = mars::stn::MQTT_MSG_PUBLISH | qos_flag;
	if(retain) {
		fixed_header[0] |= MQTT_RETAIN_FLAG;
   }
  if(dup) {
    fixed_header[0] |= MQTT_DUP_FLAG;
  }
   // Remaining Length
   if (remainLen <= 127) {
       fixed_header[1] = remainLen;
   } else {
       // first byte is remainder (mod) of 128, then set the MSB to indicate more bytes
       fixed_header[1] = remainLen % 128;
       fixed_header[1] = fixed_header[1] | 0x80;
       // second byte is number of 128s
       remainLen = remainLen >> 7;
       
       if (remainLen <= 127) {
           fixed_header[2] = remainLen;
       } else {
           // first byte is remainder (mod) of 128, then set the MSB to indicate more bytes
           fixed_header[2] = remainLen % 128;
           fixed_header[2] = fixed_header[2] | 0x80;
           // second byte is number of 128s
           fixed_header[3] = remainLen >> 7;
       }
   }

  _packed.AllocWrite(sizeof(uint8_t) * fixedHeaderSize+sizeof(uint8_t)*varHeaderSize+msglen);
  _packed.Write(fixed_header, sizeof(uint8_t) * fixedHeaderSize);
  _packed.Write(var_header, sizeof(uint8_t) * varHeaderSize);
  _packed.Write(msg, msglen);

    if (msglen > 0) {
        free(ptmp);
        msg = NULL;
        msglen = 0;
    }
    free(var_header);
    var_header = NULL;
    free(fixed_header);
    fixed_header = NULL;
    
	return 1;
}

int mqtt_puback(uint16_t message_id, AutoBuffer& _packed) {
  uint8_t packet[] = {
    mars::stn::MQTT_MSG_PUBACK | MQTT_QOS0_FLAG, // Message Type, DUP flag, QoS level, Retain
    0x02, // Remaining length
    (uint8_t)(message_id>>8),
    (uint8_t)(message_id&0xFF)
  };
  
  _packed.AllocWrite(sizeof(packet));
  _packed.Write(packet, sizeof(packet));
  
  return 1;
}

int mqtt_pubrel(uint16_t message_id, AutoBuffer& _packed) {
	uint8_t packet[] = {
		mars::stn::MQTT_MSG_PUBREL | MQTT_QOS1_FLAG, // Message Type, DUP flag, QoS level, Retain
		0x02, // Remaining length
		(uint8_t)(message_id>>8),
		(uint8_t)(message_id&0xFF)
	};

  _packed.AllocWrite(sizeof(packet));
  _packed.Write(packet, sizeof(packet));

	return 1;
}

int mqtt_subscribe(const char* topic, uint16_t message_id, AutoBuffer& _packed) {
	uint16_t topiclen = strlen(topic);

	// Variable header
	uint8_t var_header[2]; // Message ID
	var_header[0] = message_id>>8;
	var_header[1] = message_id&0xFF;

	// utf topic
    size_t topicBufSize = topiclen+3;
	uint8_t *utf_topic = (uint8_t *)malloc(sizeof(uint8_t) * topicBufSize); // Topic size (2 bytes), utf-encoded topic, QoS byte
	memset(utf_topic, 0, sizeof(uint8_t) * topicBufSize);
	utf_topic[0] = topiclen>>8;
	utf_topic[1] = topiclen&0xFF;
	memcpy(utf_topic+2, topic, topiclen);
  utf_topic[topiclen+2] = 1;

	// Fixed header
	uint8_t fixed_header[] = {
		mars::stn::MQTT_MSG_SUBSCRIBE | MQTT_QOS1_FLAG, // Message Type, DUP flag, QoS level, Retain
		(uint8_t)(sizeof(var_header)+sizeof(uint8_t) * topicBufSize)
	};

  _packed.AllocWrite(sizeof(var_header)+sizeof(fixed_header)+sizeof(uint8_t) * topicBufSize);
  _packed.Write(fixed_header, sizeof(fixed_header));
  _packed.Write(var_header, sizeof(var_header));
  _packed.Write(utf_topic, sizeof(uint8_t) * topicBufSize);
    free(utf_topic);
    utf_topic = NULL;
    return 1;
}

int mqtt_unsubscribe(const char* topic, uint16_t message_id, AutoBuffer& _packed) {
	uint16_t topiclen = strlen(topic);

	// Variable header
	uint8_t var_header[2]; // Message ID
	var_header[0] = message_id>>8;
	var_header[1] = message_id&0xFF;


	// utf topic
    size_t topicBufSize = topiclen+2;
	uint8_t *utf_topic = (uint8_t *)malloc(sizeof(uint8_t) * topicBufSize); // Topic size (2 bytes), utf-encoded topic
	memset(utf_topic, 0, sizeof(uint8_t) * topicBufSize);
	utf_topic[0] = topiclen>>8;
	utf_topic[1] = topiclen&0xFF;
	memcpy(utf_topic+2, topic, topiclen);

	// Fixed header
	uint8_t fixed_header[] = {
		mars::stn::MQTT_MSG_UNSUBSCRIBE | MQTT_QOS1_FLAG, // Message Type, DUP flag, QoS level, Retain
		(uint8_t)(sizeof(var_header)+sizeof(uint8_t) * topicBufSize)
	};

	
    _packed.AllocWrite(sizeof(var_header)+sizeof(fixed_header)+sizeof(uint8_t) * topicBufSize);
    _packed.Write(fixed_header, sizeof(fixed_header));
    _packed.Write(var_header, sizeof(var_header));
    _packed.Write(utf_topic, sizeof(uint8_t) * topicBufSize);
    free(utf_topic);
    utf_topic= NULL;
    
	return 1;
}
const static unsigned char aes_key[] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F};

unsigned char use_key[16] = {0};

//(const unsigned char* pKey, unsigned int uiKeyLen
// , const unsigned char* pInput, unsigned int uiInputLen
// , unsigned char** ppOutput, unsigned int* pOutputLen)
unsigned char * encrypt_data(const unsigned char* data, unsigned int data_length, unsigned int *output_length, bool rootKey) {
    unsigned char* output = NULL;
    
    if (rootKey || (use_key[0] + use_key[1] + use_key[2] + use_key[3] + use_key[4] == 0)) {
        if(aes_cbc_encrypt_time_check(aes_key, (unsigned int)sizeof(aes_key), data,data_length, &output, output_length) == 0) {
            return output;
        }
    } else {
        if(aes_cbc_encrypt_time_check(use_key, (unsigned int)sizeof(use_key), data,data_length, &output, output_length) == 0) {
            return output;
        }
    }
    
    
    *output_length = 0;
    return NULL;
}

unsigned char * decrypt_data(const unsigned char* data, unsigned int data_length, unsigned int *output_length, bool rootKey, bool checkTime) {
    unsigned char* output = NULL;
    if (rootKey || (use_key[0] + use_key[1] + use_key[2] + use_key[3] + use_key[4] == 0)) {
        if(aes_cbc_decrypt_time_check(aes_key, (unsigned int)sizeof(aes_key), data,data_length, &output, output_length, checkTime) == 0) {
            return output;
        }
    } else {
        if(aes_cbc_decrypt_time_check(use_key, (unsigned int)sizeof(use_key), data,data_length, &output, output_length, checkTime) == 0) {
            return output;
        }
    }
    
    *output_length = 0;
    return NULL;
}
