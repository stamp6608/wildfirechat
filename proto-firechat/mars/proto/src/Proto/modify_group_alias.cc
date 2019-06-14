//
//  modify_group_info_request.cpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "modify_group_alias.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* ModifyGroupMemberAlias::getTypeName() {
            return "ModifyGroupMemberAlias";
        }
        
        bool ModifyGroupMemberAlias::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            groupId = getString(msg, keyGroupId, 0);
            alias = getString(msg, keyAlias, 0);
            for (int i = 0; i < getSize(msg, keyToLine); i++) {
                toLine.push_back(getInt(msg, keyToLine, i));
            }
            struct pbc_rmessage *notifyMsg = getSubMessage(msg, keyNotifyContent, 0);
            notifyContent.unserializeFromPBMsg(notifyMsg, false);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void ModifyGroupMemberAlias::serializeToPBMsg(struct pbc_wmessage* msg) {
            struct pbc_wmessage *tmpMsg;
            setString(msg, keyGroupId, groupId);
            setString(msg, keyAlias, alias);
            for (std::list<int>::iterator it = toLine.begin(); it != toLine.end(); it++) {
                setInt(msg, keyToLine, *it);
            }
            tmpMsg = setSubMessaage(msg, keyNotifyContent);
            notifyContent.serializeToPBMsg(tmpMsg);
        }
    }
}
