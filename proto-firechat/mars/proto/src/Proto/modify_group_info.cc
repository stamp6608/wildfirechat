//
//  modify_group_info_request.cpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "modify_group_info.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* ModifyGroupInfoRequest::getTypeName() {
            return "ModifyGroupInfoRequest";
        }
        
        bool ModifyGroupInfoRequest::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            getString(msg, keyGroupId, 0);
            getInt(msg, keyType, 0);
            getString(msg, keyValue, 0);
            for (int i = 0; i < getSize(msg, keyToLine); i++) {
                toLine.push_back(getInt(msg, keyToLine, i));
            }
            struct pbc_rmessage *notifyMsg = getSubMessage(msg, keyNotifyContent, 0);
            notifyContent.unserializeFromPBMsg(notifyMsg, false);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void ModifyGroupInfoRequest::serializeToPBMsg(struct pbc_wmessage* msg) {
            setString(msg, keyGroupId, groupId);
            setInt(msg, keyType, type);
            setString(msg, keyValue, value);
            
            for (std::list<int>::iterator it = toLine.begin(); it != toLine.end(); it++) {
                setInt(msg, keyToLine, *it);
            }
            struct pbc_wmessage *tmpMsg = setSubMessaage(msg, keyNotifyContent);
            notifyContent.serializeToPBMsg(tmpMsg);
        }
    }
}
