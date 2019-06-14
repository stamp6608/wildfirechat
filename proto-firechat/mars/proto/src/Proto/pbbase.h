//
//  pbbase.hpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef pbbase_hpp
#define pbbase_hpp

#include <stdio.h>
#include <string>
#include "pbc/pbc.h"

struct pbc_env;
struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{

        extern const char* keyTargetUid;
        extern const char* keyReason;
        
        extern const char* keyGroupId;
        extern const char* keyAddedMember;
        extern const char* keyToLine;
        extern const char* keyNotifyContent;
        
        extern const char* keyType;
        extern const char* keyTarget;
        extern const char* keyLine;
        
        extern const char* keyGroup;
        
        
        extern const char* keyFromUid;
        extern const char* keyToUid;
        extern const char* keyStatus;
        extern const char* keyCreateDt;
        extern const char* keyUpdateDt;
        extern const char* keyFromReadStatus;
        extern const char* keyToReadStatus;
        
        extern const char* keyErrorCode;
        
        extern const char* keyDomain;
        extern const char* keyToken;
        extern const char* keyServer;
        extern const char* keyPort;
        
        extern const char* keyTargetId;
        extern const char* keyName;
        extern const char* keyPortrait;
        extern const char* keyOwner;
        extern const char* keyMemberCount;
        extern const char* keyExtra;
        extern const char* keyMemberUpdateDt;
        extern const char* keyMemberId;
        extern const char* keyAlias;
        extern const char* keyGroupInfo;
        extern const char* keyMembers;
        
        extern const char* keyId;
        
        extern const char* keyConversation;
        extern const char* keyFromUser;
        extern const char* keyToUser;
        extern const char* keyTo;
        extern const char* keyContent;
        extern const char* keyMessageId;
        extern const char* keyServerTimestamp;
        
        extern const char* keySearchableContent;
        extern const char* keyPushContent;
        extern const char* keyData;
        extern const char* keyMediaType;
        extern const char* keyRemoteMediaUrl;
    
        extern const char* kerPersistFlag;
        extern const char* keyExpireDuration;
        extern const char* keyMentionedType;
        extern const char* keyMentionedTargets;
        
        extern const char* keyValue;
        extern const char* keyEntry;

        extern const char* keyHead;
        extern const char* keyMessage;
        extern const char* keyCurrent;
        
        extern const char* keyInfo;
        
        extern const char* keyMember;
        
        extern const char* keyUid;
        extern const char* keyRequest;
        extern const char* keyUser;
        extern const char* keyCode;
        extern const char* keyResult;
        
        extern const char* keyRemovedMember;
        
        extern const char* keyNewOwner;
        
        extern const char* keyPlatform;
        extern const char* keyAppName;
        extern const char* keyApp;
        extern const char* keyDeviceToken;
        extern const char* keyPushType;
        
        extern const char* keyDisplayName;
        extern const char* keyGender;
        
        extern const char* keyMobile;
        extern const char* keyEmail;
        extern const char* keyAddress;
        extern const char* keyCompany;
        
        extern const char* keyScope;
        extern const char* keyKey;
        extern const char* keyVersion;
        
        extern const char* keyMsgHead;
        extern const char* keyFriendHead;
        extern const char* keyFriendRqHead;
        extern const char* keySettingHead;
        extern const char* keyNodeAddr;
        extern const char* keyNodePort;
        extern const char* keyServerTime;
        extern const char* keyClientId;
      
        extern const char* keyState;
        extern const char* keyKeyword;
        extern const char* keyFuzzy;
        extern const char* keyPage;
        
        extern const char* keyChatroomId;
        extern const char* keyMaxCount;
        
        extern const char* keyTitle;
        extern const char* keyDesc;
        
        
        extern const char* keyHost;
        extern const char* keyLongPort;
        extern const char* keyShortPort;
        extern const char* keySecret;
        extern const char* keySocial;
        extern const char* keyChannelId;
        extern const char* keyListen;
        extern const char* keyChannel;
        extern const char* keyCallback;
        extern const char* keyAutomatic;
        extern const char* keyDeviceName;
        extern const char* keyDeviceVersion;
        extern const char* keyPhoneName;
        extern const char* keyLanguage;
        extern const char* keyCarrierName;
        extern const char* keyAppVersion;
        extern const char* keySdkVersion;
        
        extern const char* keyBeforeUid;
        extern const char* keyCount;
        
            class PBBase {                
            private:
                static struct pbc_env *m_env;
                
            protected:
                virtual const char* getTypeName() = 0;
                
                //read
                struct pbc_rmessage* prepareRead(const void *data, int length);
                unsigned int getInt(struct pbc_rmessage * , const char *key , int index);
                int64_t getInt64(struct pbc_rmessage * , const char *key , int index);
                double getReal(struct pbc_rmessage * , const char *key , int index);
                std::string getString(struct pbc_rmessage * , const char *key , int index);
                const char* getData(struct pbc_rmessage * , const char *key , int index, int *sz);
                struct pbc_rmessage * getSubMessage(struct pbc_rmessage *, const char *key, int index);
                int getSize(struct pbc_rmessage *, const char *key);
                int getNext(struct pbc_rmessage *, const char **key);
                void finishRead(struct pbc_rmessage *);
                
                //write
                struct pbc_wmessage* prepareWrite();
                int setInt(struct pbc_wmessage *, const char *key, int value);
                int setInt64(struct pbc_wmessage *, const char *key, int64_t value);
                int setReal(struct pbc_wmessage *, const char *key, double v);
                int setString(struct pbc_wmessage *, const char *key, const std::string &value);
                int setData(struct pbc_wmessage *, const char *key, const char * v, int len);
                struct pbc_wmessage * setSubMessaage(struct pbc_wmessage *, const char *key);
                const std::string finishWrite(struct pbc_wmessage *);
                
                
            public:
                PBBase() {}
                virtual ~PBBase() {}
                
                const std::string serializeToPBData();
                bool unserializeFromPBData(const void* data, int length);
                
                virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) = 0;
                virtual void serializeToPBMsg(struct pbc_wmessage* msg) = 0;
            };
        }
}
#endif /* pbbase_hpp */
