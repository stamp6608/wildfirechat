//
//  add_group_member_request.cpp
//  proto
//
//  Created by WF Chat on 2017/11/30.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "add_group_member_request.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* AddGroupMemberRequest::getTypeName() {
            return "AddGroupMemberRequest";
        }
        
        bool AddGroupMemberRequest::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            groupId = getString(msg, keyGroupId, 0);
            int size = getSize(msg, keyAddedMember);
            for (int i = 0; i < size; i++) {
                struct pbc_rmessage *addedMemberMsg = getSubMessage(msg, keyAddedMember, i);
                GroupMember member;
                member.unserializeFromPBMsg(addedMemberMsg, false);
                addedMembers.push_back(member);
            }
            size = getSize(msg, keyToLine);
            for (int i = 0; i < size; i++) {
                int line = getInt(msg, keyToLine, i);
                toLines.push_back(line);
            }
            struct pbc_rmessage *notifyContentMsg = getSubMessage(msg, keyNotifyContent, 0);
            notifyContent.unserializeFromPBMsg(notifyContentMsg, false);
            
            if(destroy)
            finishRead(msg);
            
            return true;
        }
        
        void AddGroupMemberRequest::serializeToPBMsg(struct pbc_wmessage* msg) {
            setString(msg, keyGroupId, groupId);
            for (std::list<GroupMember>::iterator it = addedMembers.begin(); it != addedMembers.end(); it++) {
                struct pbc_wmessage *addedMemberMsg = setSubMessaage(msg, keyAddedMember);
                (*it).serializeToPBMsg(addedMemberMsg);
            }
            for (int i = 0; i < toLines.size(); i++) {
                setInt(msg, keyToLine, toLines[i]);
            }
            struct pbc_wmessage *notifyContentMsg = setSubMessaage(msg, keyNotifyContent);
            notifyContent.serializeToPBMsg(notifyContentMsg);
        }
    }
}
