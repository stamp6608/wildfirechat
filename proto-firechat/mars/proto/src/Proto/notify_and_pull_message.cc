//
//  notify_and_pull_message.cpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "notify_and_pull_message.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* NotifyMessage::getTypeName() {
            return "NotifyMessage";
        }
        
        bool NotifyMessage::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            type = (PullType)getInt(msg, keyType, 0);
            head = getInt64(msg, keyHead, 0);
            target = getString(msg, keyTarget, 0);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void NotifyMessage::serializeToPBMsg(struct pbc_wmessage* msg) {
            setInt(msg, keyType, type);
            setInt64(msg, keyHead, head);
            setString(msg, keyTarget, target);
        }
        
        const char* PullMessageRequest::getTypeName() {
            return "PullMessageRequest";
        }
        
        bool PullMessageRequest::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            type = (PullType)getInt(msg, keyType, 0);
            id = getInt64(msg, keyId, 0);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void PullMessageRequest::serializeToPBMsg(struct pbc_wmessage* msg) {
            setInt(msg, keyType, type);
            setInt64(msg, keyId, id);
        }

        const char* PullMessageResult::getTypeName() {
            return "PullMessageResult";
        }
        
        bool PullMessageResult::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            for (int i = 0; i < getSize(msg, keyMessage); i++) {
                struct pbc_rmessage *tmpMsg = getSubMessage(msg, keyMessage, i);
                Message message;
                message.unserializeFromPBMsg(tmpMsg, false);
                messages.push_back(message);
            }
            
            
            current = getInt64(msg, keyCurrent, 0);
            head = getInt64(msg, keyHead, 0);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void PullMessageResult::serializeToPBMsg(struct pbc_wmessage* msg) {
            
        }

    }
}
