//
//  pull_group_info_result.cpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "pull_group_info_result.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{        

        const char* PullGroupInfoResult::getTypeName() {
            return "PullGroupInfoResult";
        }
        
        bool PullGroupInfoResult::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            for (int i = 0; i < getSize(msg, keyInfo); i++) {
                struct pbc_rmessage *tmpMsg = getSubMessage(msg, keyInfo, i);
                GroupInfo groupInfo;
                groupInfo.unserializeFromPBMsg(tmpMsg, false);
                infos.push_back(groupInfo);
            }
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void PullGroupInfoResult::serializeToPBMsg(struct pbc_wmessage* msg) {
            for (std::list<GroupInfo>::iterator it = infos.begin(); it != infos.end(); it++) {
                struct pbc_wmessage *tmpMsg = setSubMessaage(msg, keyInfo);
                (*it).serializeToPBMsg(tmpMsg);
            }
        }
    }
}
