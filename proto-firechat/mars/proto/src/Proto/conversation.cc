//
//  conversation.cpp
//  proto
//
//  Created by WF Chat on 2017/11/29.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "conversation.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* Conversation::getTypeName() {
            return "Conversation";
        }
        
        bool Conversation::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            type = (ConversationType)getInt(msg, keyType, 0);
            target = getString(msg, keyTarget, 0);
            line = getInt(msg, keyLine, 0);
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void Conversation::serializeToPBMsg(struct pbc_wmessage* msg)  {
            setInt(msg, keyType, type);
            setString(msg, keyTarget, target);
            setInt(msg, keyLine, line);
        }
    }
}
