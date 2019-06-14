//
//  handle_friends_request.cpp
//  proto
//
//  Created by WF Chat on 2017/12/1.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "handle_friends_request.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* HandleFriendRequest::getTypeName() {
            return "HandleFriendRequest";
        }
        
        bool HandleFriendRequest::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            targetUid = getString(msg, keyTargetUid, 0);
            status = getInt(msg, keyStatus, 0);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void HandleFriendRequest::serializeToPBMsg(struct pbc_wmessage* msg) {
            setString(msg, keyTargetUid, targetUid);
            setInt(msg, keyStatus, status);
        }
    }
}
