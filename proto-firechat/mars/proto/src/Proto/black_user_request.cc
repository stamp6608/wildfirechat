//
//  add_friends_request.cpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "black_user_request.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* BlackUserRequest::getTypeName() {
            return "BlackUserRequest";
        }
        
        bool BlackUserRequest::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            userId = getString(msg, keyUid, 0);
            status = getInt(msg, keyStatus, 0);
            if (destroy) {
                finishRead(msg);
            }
            return true;
        }
        
        void BlackUserRequest::serializeToPBMsg(struct pbc_wmessage* msg) {
            setString(msg, keyUid, userId);
            setInt(msg, keyStatus, status);
        }
    }
}
