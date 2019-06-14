//
//  create_group_request.cpp
//  proto
//
//  Created by WF Chat on 2017/11/30.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "create_group_request.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{

        const char* CreateGroupRequest::getTypeName() {
            return "CreateGroupRequest";
        }
        
        bool CreateGroupRequest::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            struct pbc_rmessage *groupMsg = getSubMessage(msg, keyGroup, 0);
            group.unserializeFromPBMsg(groupMsg, false);
            int size = getSize(msg, keyToLine);
            for (int i = 0; i < size; i++) {
                int value = getInt(msg, keyToLine, i);
                toLines.push_back(value);
            }
            struct pbc_rmessage *notifyContentMsg = getSubMessage(msg, keyNotifyContent, 0);
            notifyContent.unserializeFromPBMsg(notifyContentMsg, false);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void CreateGroupRequest::serializeToPBMsg(struct pbc_wmessage* msg) {
            struct pbc_wmessage *groupMsg = setSubMessaage(msg, keyGroup);
            group.serializeToPBMsg(groupMsg);
            for (int i = 0; i < toLines.size(); i++) {
                setInt(msg, keyToLine, toLines[i]);
            }
            struct pbc_wmessage *notifyContentMsg = setSubMessaage(msg, keyNotifyContent);
            notifyContent.serializeToPBMsg(notifyContentMsg);
        }
    }
}
