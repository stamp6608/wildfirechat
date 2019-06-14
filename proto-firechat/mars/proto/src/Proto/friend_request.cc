//
//  friend_request.cpp
//  proto
//
//  Created by WF Chat on 2017/11/29.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "friend_request.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* FriendRequest::getTypeName() {
            return "FriendRequest";
        }
        
        bool FriendRequest::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            fromUid = getString(msg, keyFromUid, 0);
            toUid = getString(msg, keyToUid, 0);
            reason = getString(msg, keyReason, 0);
            status = (FriendRequestStatus)getInt(msg, keyStatus, 0);
            updateDt = getInt64(msg, keyUpdateDt, 0);
            fromReadStatus = getInt(msg, keyFromReadStatus, 0);
            toReadStatus = getInt(msg, keyToReadStatus, 0);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void FriendRequest::serializeToPBMsg(struct pbc_wmessage* msg)  {
            setString(msg, keyFromUid, fromUid);
            setString(msg, keyToUid, toUid);
            setString(msg, keyReason, reason);
            setInt(msg, keyStatus, status);
            setInt64(msg, keyUpdateDt, updateDt);
            setInt(msg, keyFromReadStatus, fromReadStatus);
            setInt(msg, keyToReadStatus, toReadStatus);
        }
    }
}
