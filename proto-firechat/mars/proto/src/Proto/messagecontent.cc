//
//  MessageContent.cpp
//  proto
//
//  Created by WF Chat on 2017/11/30.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "messagecontent.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* MessageContent::getTypeName() {
            return "MessageContent";
        }
        
        
        bool MessageContent::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            type = getInt(msg, keyType, 0);
            searchableContent = getString(msg, keySearchableContent, 0);
            pushContent = getString(msg, keyPushContent, 0);
            content = getString(msg, keyContent, 0);
            data = getString(msg, keyData, 0);
            mediaType = getInt(msg, keyMediaType, 0);
            remoteMediaUrl = getString(msg, keyRemoteMediaUrl, 0);
            
            
            persistFlag = getInt(msg, kerPersistFlag, 0);
            expireDuration = getInt(msg, keyExpireDuration, 0);
            mentionedType = getInt(msg, keyMentionedType, 0);
            int size = getSize(msg, keyMentionedTargets);
            for (int i = 0; i < size; i++) {
                mentionedTargets.insert(mentionedTargets.end(), getString(msg, keyMentionedTargets, i));
            }
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void MessageContent::serializeToPBMsg(struct pbc_wmessage* msg) {
            setInt(msg, keyType, type);
            setString(msg, keySearchableContent, searchableContent);
            setString(msg, keyPushContent, pushContent);
            setString(msg, keyContent, content);
            setString(msg, keyData, data);
            setInt(msg, keyMediaType, mediaType);
            setString(msg, keyRemoteMediaUrl, remoteMediaUrl);
            setInt(msg, kerPersistFlag, persistFlag);
            setInt(msg, keyExpireDuration, expireDuration);
            setInt(msg, keyMentionedType, mentionedType);
            for (std::list<std::string>::iterator it = mentionedTargets.begin(); it != mentionedTargets.end(); it++) {
                setString(msg, keyMentionedTargets, *it);
            }
        }
    }
}
