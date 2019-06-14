//
//  quit_group_request.cpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "get_chatroom_info_request.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* GetChatroomInfoRequest::getTypeName() {
            return "GetChatroomInfoRequest";
        }
        
        bool GetChatroomInfoRequest::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            chatroomId = getString(msg, keyChatroomId, 0);
            updateDt = getInt64(msg, keyUpdateDt, 0);
            if(destroy)
                finishRead(msg);
            return true;
        }
        
        void GetChatroomInfoRequest::serializeToPBMsg(struct pbc_wmessage* msg) {
            setString(msg, keyChatroomId, chatroomId);
            setInt64(msg, keyUpdateDt, updateDt);
        }
    }
}
