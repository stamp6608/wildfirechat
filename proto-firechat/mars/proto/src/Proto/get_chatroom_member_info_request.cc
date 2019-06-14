//
//  quit_group_request.cpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "get_chatroom_member_info_request.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* GetChatroomMemberInfoRequest::getTypeName() {
            return "GetChatroomMemberInfoRequest";
        }
        
        bool GetChatroomMemberInfoRequest::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            chatroomId = getString(msg, keyChatroomId, 0);
            maxCount = getInt(msg, keyMaxCount, 0);
            if(destroy)
                finishRead(msg);
            return true;
        }
        
        void GetChatroomMemberInfoRequest::serializeToPBMsg(struct pbc_wmessage* msg) {
            setString(msg, keyChatroomId, chatroomId);
            setInt(msg, keyMaxCount, maxCount);
        }
    }
}
