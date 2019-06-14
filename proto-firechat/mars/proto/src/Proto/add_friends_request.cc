//
//  add_friends_request.cpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "add_friends_request.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* AddFriendRequest::getTypeName() {
            return "AddFriendRequest";
        }
        
        bool AddFriendRequest::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            targetUid = getString(msg, keyTargetUid, 0);
            reason = getString(msg, keyReason, 0);
            if (destroy) {
                finishRead(msg);
            }
            return true;
        }
        
        void AddFriendRequest::serializeToPBMsg(struct pbc_wmessage* msg) {
            setString(msg, keyTargetUid, targetUid);
            setString(msg, keyReason, reason);
        }
    }
}
