//
//  add_friends_request.cpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "search_user_result.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
      
        const char* SearchUserResult::getTypeName() {
            return "SearchUserResult";
        }
        
        bool SearchUserResult::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            for (int i = 0; i < getSize(msg, keyEntry); i++) {
                struct pbc_rmessage *tmpMsg = getSubMessage(msg, keyEntry, i);
                User result;
                result.unserializeFromPBMsg(tmpMsg, false);
                entrys.push_back(result);
            }
            if (destroy) {
                finishRead(msg);
            }
            return true;
        }
        
        void SearchUserResult::serializeToPBMsg(struct pbc_wmessage* msg) {
            for (std::list<User>::iterator it = entrys.begin(); it != entrys.end(); it++) {
                struct pbc_wmessage *tmpMsg = setSubMessaage(msg, keyEntry);
                (*it).serializeToPBMsg(tmpMsg);
            }
        }
    }
}
