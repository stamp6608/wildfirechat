//
//  add_friends_request.cpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "friend.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* Friend::getTypeName() {
            return "Friend";
        }
        
        bool Friend::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            state = getInt(msg, keyState, 0);
            uid = getString(msg, keyUid, 0);
            updateDt = getInt64(msg, keyUpdateDt, 0);
            alias = getString(msg, keyAlias, 0);
            if (destroy) {
                finishRead(msg);
            }
            return true;
        }
        
        void Friend::serializeToPBMsg(struct pbc_wmessage* msg) {
            setInt(msg, keyState, state);
            setString(msg, keyUid, uid);
            setInt64(msg, keyUpdateDt, updateDt);
            setString(msg, keyAlias, alias);
        }
    }
}
