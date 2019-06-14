//
//  remove_group_member_request.cpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "remove_group_member_request.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* RemoveGroupMemberRequest::getTypeName() {
            return "RemoveGroupMemberRequest";
        }
        
        bool RemoveGroupMemberRequest::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            groupId = getString(msg, keyGroupId, 0);
            for (int i = 0; i < getSize(msg, keyRemovedMember); i++) {
                removedMembers.push_back(getString(msg, keyRemovedMember, i));
            }
            for (int j = 0; j < getSize(msg, keyToLine); j++) {
                toLines.push_back(getInt(msg, keyToLine, j));
            }
            struct pbc_rmessage *tmpMsg = getSubMessage(msg, keyNotifyContent, 0);
            notifyContent.unserializeFromPBMsg(tmpMsg, false);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void RemoveGroupMemberRequest::serializeToPBMsg(struct pbc_wmessage* msg) {
            setString(msg, keyGroupId, groupId);
            for (std::list<std::string>::iterator it = removedMembers.begin(); it != removedMembers.end(); it++) {
                setString(msg, keyRemovedMember, *it);
            }
            for (std::list<int>::iterator it2 = toLines.begin(); it2 != toLines.end(); it2++) {
                setInt(msg, keyToLine, *it2);
            }
            struct pbc_wmessage *tmpMsg = setSubMessaage(msg, keyNotifyContent);
            notifyContent.serializeToPBMsg(tmpMsg);
        }
    }
}
