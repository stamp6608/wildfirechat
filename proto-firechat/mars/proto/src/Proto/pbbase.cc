//
//  pbbase.cpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "pbbase.h"
#include <string>
#include "pbc/pbc.h"
#include "pbdata.h"
#include "mars/comm/xlogger/xlogger.h"

namespace mars{
    namespace stn{
        const char* keyIMToken = "imtoken";
        
        const char* keyTargetUid = "target_uid";
        const char* keyReason = "reason";

        const char* keyGroupId = "group_id";
        const char* keyAddedMember = "added_member";
        const char* keyToLine = "to_line";
        const char* keyNotifyContent = "notify_content";
        
        const char* keyType = "type";
        const char* keyTarget = "target";
        const char* keyLine = "line";
        
        const char* keyGroup = "group";
        
        const char* keyFromUid = "from_uid";
        const char* keyToUid = "to_uid";
        const char* keyStatus = "status";
        const char* keyCreateDt = "create_dt";
        const char* keyUpdateDt = "update_dt";
        const char* keyFromReadStatus = "from_read_status";
        const char* keyToReadStatus = "to_read_status";
        
        const char* keyErrorCode = "error_code";
        
        const char* keyDomain = "domain";
        const char* keyToken = "token";
        const char* keyServer = "server";
        const char* keyPort = "port";
        
        
        const char* keyTargetId = "target_id";
        const char* keyName = "name";
        const char* keyPortrait = "portrait";
        const char* keyOwner = "owner";
        const char* keyMemberCount = "member_count";
        const char* keyExtra = "extra";
        const char* keyMemberUpdateDt = "member_update_dt";
        const char* keyMemberId = "member_id";
        const char* keyAlias = "alias";
        const char* keyGroupInfo = "group_info";
        const char* keyMembers = "members";
        
        const char* keyId = "id";
        
        const char* keyConversation = "conversation";
        const char* keyFromUser = "from_user";
        const char* keyToUser = "to_user";
        const char* keyTo = "to";
        const char* keyContent = "content";
        const char* keyMessageId = "message_id";
        const char* keyServerTimestamp = "server_timestamp";
        
        const char* keySearchableContent = "searchable_content";
        const char* keyPushContent = "push_content";
        const char* keyData = "data";
        const char* keyMediaType = "mediaType";
        const char* keyRemoteMediaUrl = "remoteMediaUrl";
        
        const char* kerPersistFlag = "persist_flag";
        const char* keyExpireDuration = "expire_duration";
        const char* keyMentionedType = "mentioned_type";
        const char* keyMentionedTargets = "mentioned_target";
        
        const char* keyValue = "value";
        const char* keyEntry = "entry";

        
        const char* keyHead = "head";
        const char* keyMessage = "message";
        const char* keyCurrent = "current";
        
        const char* keyInfo = "info";
                
        const char* keyMember = "member";
        
        
        const char* keyUid = "uid";
        const char* keyRequest = "request";
        const char* keyUser = "user";
        const char* keyCode = "code";
        const char* keyResult = "result";
        
        const char* keyRemovedMember = "removed_member";
        
        const char* keyNewOwner = "new_owner";
        
        const char* keyPlatform = "platform";
        const char* keyAppName = "app_name";
        const char* keyApp = "app";
        const char* keyDeviceToken = "device_token";
        const char* keyPushType = "push_type";
        
        const char* keyDisplayName = "display_name";
        const char* keyGender = "gender";
        
        const char* keyMobile = "mobile";
        const char* keyEmail = "email";
        const char* keyAddress = "address";
        const char* keyCompany = "company";
        
        const char* keyScope = "scope";
        const char* keyKey = "key";
        
        const char* keyVersion = "version";
      
        const char* keyMsgHead = "msg_head";
        const char* keyFriendHead = "friend_head";
        const char* keyFriendRqHead = "friend_rq_head";
        const char* keySettingHead = "setting_head";
        const char* keyNodeAddr = "node_addr";
        const char* keyNodePort = "node_port";
        
        const char* keyServerTime = "server_time";
        const char* keyClientId = "client_id";
        const char* keyState = "state";
        const char* keyKeyword = "keyword";
        const char* keyFuzzy = "fuzzy";
        const char* keyPage = "page";
        
        const char* keyChatroomId = "chatroom_id";
        const char* keyMaxCount = "max_count";
        
        const char* keyTitle = "title";
        const char* keyDesc = "desc";
        
        const char* keyHost = "host";
        const char* keyLongPort = "long_port";
        const char* keyShortPort = "short_port";
        const char* keySecret = "secret";
        const char* keySocial = "social";
        const char* keyCallback = "callback";
        const char* keyAutomatic = "automatic";
        
        
        const char* keyChannel = "channel";
        const char* keyChannelId = "channel_id";
        const char* keyListen = "listen";
        
        
        const char* keyDeviceName = "device_name";
        const char* keyDeviceVersion = "device_version";
        const char* keyPhoneName = "phone_name";
        const char* keyLanguage = "language";
        const char* keyCarrierName = "carrier_name";
        const char* keyAppVersion = "app_version";
        const char* keySdkVersion = "sdk_version";
        const char* keyBeforeUid = "before_uid";
        const char* keyCount = "count";
        
        pbc_env* initEnv() {
            struct pbc_slice slice;
            slice.len = sizeof(pbdata);
            slice.buffer = (void*)pbdata;
            pbc_env* env = pbc_new();
            pbc_register(env, &slice);
            return env;
        }
        
        struct pbc_env* PBBase::m_env = initEnv();
        
        
        struct pbc_rmessage* PBBase::prepareRead(const void *data, int length) {
            struct pbc_slice slice;
            slice.buffer = (void*)data;
            slice.len = (int)length;
            struct pbc_rmessage *msg = pbc_rmessage_new(m_env, getTypeName(), &slice);
            return msg;
        }
        unsigned int PBBase::getInt(struct pbc_rmessage * msg, const char *key , int index) {
            int signNumber = 0;
            unsigned int number = pbc_rmessage_integer(msg, key, index, (unsigned int *)&signNumber);
            if(signNumber == -1) {
                return -number;
            }
            return number;
        }
        
        int64_t PBBase::getInt64(struct pbc_rmessage * msg, const char *key , int index) {
            unsigned int hi = 0;
            unsigned low = pbc_rmessage_integer(msg, key, index, &hi);
            int64_t value = hi;
            value = value<<32;
            value += low;
            
            return value;
        }
        double PBBase::getReal(struct pbc_rmessage * msg, const char *key , int index) {
            return pbc_rmessage_real(msg, key, index);
        }
        
        std::string PBBase::getString(struct pbc_rmessage *msg, const char *key , int index) {
            int len = 0;
            const char * buf = pbc_rmessage_string(msg, key, index, &len);
            return std::string(buf, len);
        }
        
        const char * PBBase::getData(struct pbc_rmessage *msg , const char *key , int index, int *sz) {
            return pbc_rmessage_string(msg, key, index, sz);
        }
        
        struct pbc_rmessage * PBBase::getSubMessage(struct pbc_rmessage *msg, const char *key, int index) {
            return pbc_rmessage_message(msg, key, index);
        }
        
        int PBBase::getSize(struct pbc_rmessage *msg, const char *key) {
            return pbc_rmessage_size(msg, key);
        }
        
        int PBBase::getNext(struct pbc_rmessage *msg, const char **key) {
            return pbc_rmessage_next(msg, key);
        }
        
        void PBBase::finishRead(struct pbc_rmessage *msg) {
            pbc_rmessage_delete(msg);
        }
        
        //write
        struct pbc_wmessage* PBBase::prepareWrite() {
            return pbc_wmessage_new(m_env, getTypeName());
        }
        
        int PBBase::setInt(struct pbc_wmessage *msg, const char *key, int value) {
            unsigned int low = abs(value);
            unsigned int hi = 0;
            if (value < 0) {
                hi = -1;
            }
            return pbc_wmessage_integer(msg, key, low, hi);
        }
        int PBBase::setInt64(struct pbc_wmessage *msg, const char *key, int64_t value) {
            unsigned int hi = value >> 32;
            int64_t hi64 = hi;
            unsigned int low = (unsigned int)(value - (hi64 << 32));
            
            return pbc_wmessage_integer(msg, key, low, hi);
        }
        
        int PBBase::setReal(struct pbc_wmessage *msg, const char *key, double v) {
            return pbc_wmessage_real(msg, key, v);
        }
        int PBBase::setString(struct pbc_wmessage *msg, const char *key, const std::string &value) {
            return pbc_wmessage_string(msg, key, value.c_str(), (int)value.length());
        }
        
        int PBBase::setData(struct pbc_wmessage *msg, const char *key, const char * v, int len) {
            return pbc_wmessage_string(msg, key, v, len);
        }
        struct pbc_wmessage* PBBase::setSubMessaage(struct pbc_wmessage *msg, const char *key) {
            return pbc_wmessage_message(msg, key);
        }
        const std::string PBBase::finishWrite(struct pbc_wmessage *msg) {
            struct pbc_slice slice;
            pbc_wmessage_buffer(msg, &slice);
            
            std::string result = std::string((const char *)slice.buffer, slice.len);
            pbc_wmessage_delete(msg);
            return result;
        }
        
        bool PBBase::unserializeFromPBData(const void* data, int length) {
            struct pbc_rmessage *msg = prepareRead(data, length);
            if (msg == NULL) {
                xinfo2(TSF"PROTO -> Decode PB failure %0)", getTypeName());
                return false;
            }
            return unserializeFromPBMsg(msg, true);
        }
        
        const std::string PBBase::serializeToPBData() {
            struct pbc_wmessage *msg = prepareWrite();
            serializeToPBMsg(msg);
            return finishWrite(msg);
        }
    }
}
