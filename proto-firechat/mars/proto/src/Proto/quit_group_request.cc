//
//  quit_group_request.cpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "quit_group_request.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* QuitGroupRequest::getTypeName() {
            return "QuitGroupRequest";
        }
        
        bool QuitGroupRequest::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            groupId = getString(msg, keyGroupId, 0);
            for (int i = 0; i < getSize(msg, keyToLine); i++) {
                toLines.push_back(getInt(msg, keyToLine, i));
            }
            struct pbc_rmessage *tmpMsg = getSubMessage(msg, keyNotifyContent, 0);
            notifyContent.unserializeFromPBMsg(tmpMsg, false);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void QuitGroupRequest::serializeToPBMsg(struct pbc_wmessage* msg) {
            setString(msg, keyGroupId, groupId);
            for (std::list<int>::iterator it = toLines.begin(); it != toLines.end(); it++) {
                setInt(msg, keyToLine, *it);
            }
            static pbc_wmessage *tmpMsg = NULL;
            tmpMsg = setSubMessaage(msg, keyNotifyContent);
            notifyContent.serializeToPBMsg(tmpMsg);
        }
    }
}
