//
//  quit_group_request.cpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "get_chatroom_info_result.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* ChatroomInfo::getTypeName() {
            return "ChatroomInfo";
        }
        
        
        bool ChatroomInfo::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            title = getString(msg, keyTitle, 0);
            desc = getString(msg, keyDesc, 0);
            portrait = getString(msg, keyPortrait, 0);
            memberCount = getInt(msg, keyMemberCount, 0);
            createDt = getInt64(msg, keyCreateDt, 0);
            updateDt = getInt64(msg, keyUpdateDt, 0);
            extra = getString(msg, keyExtra, 0);
            state = (ChatroomState)getInt(msg, keyState, 0);
            if(destroy)
                finishRead(msg);
            return true;
        }
        
        void ChatroomInfo::serializeToPBMsg(struct pbc_wmessage* msg) {
            setString(msg, keyTitle, title);
            setString(msg, keyDesc, desc);
            setString(msg, keyPortrait, portrait);
            setInt(msg, keyMemberCount, memberCount);
            setInt64(msg, keyCreateDt, createDt);
            setInt64(msg, keyUpdateDt, updateDt);
            setString(msg, keyExtra, extra);
            setInt(msg, keyState, (int)state);
        }
    }
}
