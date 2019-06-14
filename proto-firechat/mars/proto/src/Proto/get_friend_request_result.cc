//
//  add_friends_request.cpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "get_friend_request_result.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
      
        const char* GetFriendRequestResult::getTypeName() {
            return "GetFriendRequestResult";
        }
        
        bool GetFriendRequestResult::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            for (int i = 0; i < getSize(msg, keyEntry); i++) {
                struct pbc_rmessage *tmpMsg = getSubMessage(msg, keyEntry, i);
                FriendRequest result;
                result.unserializeFromPBMsg(tmpMsg, false);
                entrys.push_back(result);
            }
            if (destroy) {
                finishRead(msg);
            }
            return true;
        }
        
        void GetFriendRequestResult::serializeToPBMsg(struct pbc_wmessage* msg) {
            for (std::list<FriendRequest>::iterator it = entrys.begin(); it != entrys.end(); it++) {
                struct pbc_wmessage *tmpMsg = setSubMessaage(msg, keyEntry);
                (*it).serializeToPBMsg(tmpMsg);
            }
        }
    }
}
