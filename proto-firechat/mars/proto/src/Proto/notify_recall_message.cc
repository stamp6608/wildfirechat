//
//  notify_recall_message.cpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "notify_recall_message.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* NotifyRecallMessage::getTypeName() {
            return "NotifyRecallMessage";
        }
        
        bool NotifyRecallMessage::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            messageUid = getInt64(msg, keyId, 0);
            operatorId = getString(msg, keyFromUser, 0);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void NotifyRecallMessage::serializeToPBMsg(struct pbc_wmessage* msg) {
            setInt64(msg, keyId, messageUid);
            setString(msg, keyFromUser, operatorId);
        }
    }
}
