//
//  message.cpp
//  proto
//
//  Created by WF Chat on 2017/12/1.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "load_remote_messages.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
            
        const char* LoadRemoteMessages::getTypeName() {
            return "LoadRemoteMessages";
        }
        
        bool LoadRemoteMessages::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            struct pbc_rmessage *convMsg = getSubMessage(msg, keyConversation, 0);
            conversation.unserializeFromPBMsg(convMsg, false);
            
            
            beforeUid = getInt64(msg, keyBeforeUid, 0);
            count = getInt(msg, keyCount, 0);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void LoadRemoteMessages::serializeToPBMsg(struct pbc_wmessage* msg) {
            struct pbc_wmessage *convMsg = setSubMessaage(msg, keyConversation);
            conversation.serializeToPBMsg(convMsg);
            
            setInt64(msg, keyBeforeUid, beforeUid);
            setInt(msg, keyCount, count);
        }
    }
}
