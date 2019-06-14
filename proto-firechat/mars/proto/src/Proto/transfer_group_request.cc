//
//  transfer_group_request.cpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "transfer_group_request.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{

        const char* TransferGroupRequest::getTypeName() {
            return "TransferGroupRequest";
        }
        
        bool TransferGroupRequest::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            groupId = getString(msg, keyGroupId, 0);
            newOwner = getString(msg, keyNewOwner, 0);
            for (int j = 0; j < getSize(msg, keyToLine); j++) {
                toLines.push_back(getInt(msg, keyToLine, j));
            }
            struct pbc_rmessage *tmpMsg = getSubMessage(msg, keyNotifyContent, 0);
            notifyContent.unserializeFromPBMsg(tmpMsg, false);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void TransferGroupRequest::serializeToPBMsg(struct pbc_wmessage* msg) {
            setString(msg, keyGroupId, groupId);
            setString(msg, keyNewOwner, newOwner);
            for (std::list<int>::iterator it2 = toLines.begin(); it2 != toLines.end(); it2++) {
                setInt(msg, keyToLine, *it2);
            }
            struct pbc_wmessage *tmpMsg = setSubMessaage(msg, keyNotifyContent);
            notifyContent.serializeToPBMsg(tmpMsg);
        }
    }
}
