//
//  pull_group_member.cpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "pull_group_member.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* PullGroupMemberRequest::getTypeName() {
            return "PullGroupMemberRequest";
        }
        
        bool PullGroupMemberRequest::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            target = getString(msg, keyTarget, 0);
            head = getInt64(msg, keyHead, 0);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void PullGroupMemberRequest::serializeToPBMsg(struct pbc_wmessage* msg) {
            setString(msg, keyTarget, target);
            setInt64(msg, keyHead, head);
        }
        
        
        const char* PullGroupMemberResult::getTypeName() {
            return "PullGroupMemberResult";
        }
        
        bool PullGroupMemberResult::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            for (int i = 0; i < getSize(msg, keyMember); i++) {
                struct pbc_rmessage *tmpMsg = getSubMessage(msg, keyMember, i);
                GroupMember groupMember;
                groupMember.unserializeFromPBMsg(tmpMsg, false);
                members.push_back(groupMember);
            }
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void PullGroupMemberResult::serializeToPBMsg(struct pbc_wmessage* msg) {
            for (std::list<GroupMember>::iterator it = members.begin(); it != members.end(); it++) {
                struct pbc_wmessage *tmpMsg = setSubMessaage(msg, keyMember);
                (*it).serializeToPBMsg(tmpMsg);
            }
        }

    }
}
