//
//  add_friends_request.cpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "version.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* Version::getTypeName() {
            return "Version";
        }
        
        bool Version::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            version = getInt64(msg, keyVersion, 0);
            if (destroy) {
                finishRead(msg);
            }
            return true;
        }
        
        void Version::serializeToPBMsg(struct pbc_wmessage* msg) {
            setInt64(msg, keyVersion, version);
        }
    }
}
