//
//  message.cpp
//  proto
//
//  Created by WF Chat on 2017/12/1.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "message.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
            
        const char* Message::getTypeName() {
            return "Message";
        }
        
        bool Message::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            struct pbc_rmessage *convMsg = getSubMessage(msg, keyConversation, 0);
            conversation.unserializeFromPBMsg(convMsg, false);
            fromUser = getString(msg, keyFromUser, 0);
            struct pbc_rmessage *contentMsg = getSubMessage(msg, keyContent, 0);
            content.unserializeFromPBMsg(contentMsg, false);
            messageId = getInt64(msg, keyMessageId, 0);
            serverTimestamp = getInt64(msg, keyServerTimestamp, 0);
            for (int i = 0; i < getSize(msg, keyId); i++) {
                std::string value = getString(msg, keyTo, i);
                tos.push_back(value);
            }
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void Message::serializeToPBMsg(struct pbc_wmessage* msg) {
            struct pbc_wmessage *convMsg = setSubMessaage(msg, keyConversation);
            conversation.serializeToPBMsg(convMsg);
            
            setString(msg, keyFromUser, fromUser);
            
            struct pbc_wmessage *contentMsg = setSubMessaage(msg, keyContent);
            content.serializeToPBMsg(contentMsg);
            
            setInt64(msg, keyMessageId, messageId);
            setInt64(msg, keyServerTimestamp, serverTimestamp);

            for (std::list<std::string>::iterator it = tos.begin(); it != tos.end(); it++) {
                setString(msg, keyTo, *it);
            }
            
        }
    }
}
