//
//  quit_group_request.cpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "get_chatroom_member_info_result.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* ChatroomMemberInfo::getTypeName() {
            return "ChatroomMemberInfo";
        }
        
        bool ChatroomMemberInfo::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            memberCount = getInt(msg, keyMemberCount, 0);
            
            for (int i = 0; i < getSize(msg, keyId); i++) {
                std::string value = getString(msg, keyMembers, i);
                members.push_back(value);
            }
            
            if(destroy)
                finishRead(msg);
            return true;
        }
        
        void ChatroomMemberInfo::serializeToPBMsg(struct pbc_wmessage* msg) {
            setInt(msg, keyMemberCount, memberCount);
            
            for (std::list<std::string>::iterator it = members.begin(); it != members.end(); it++) {
                setString(msg, keyMembers, *it);
            }
        }
    }
}
