//
//  id_buf.cpp
//  proto
//
//  Created by WF Chat on 2017/12/1.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "int64_buf.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        const char* INT64Buf::getTypeName() {
            return "INT64Buf";
        }
        
        bool INT64Buf::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            id = getInt64(msg, keyId, 0);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void INT64Buf::serializeToPBMsg(struct pbc_wmessage* msg) {
            setInt64(msg, keyId, id);
        }
    }
}
