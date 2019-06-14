//
//  id_buf.cpp
//  proto
//
//  Created by WF Chat on 2017/12/1.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "id_buf.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        const char* IDBuf::getTypeName() {
            return "IDBuf";
        }
        
        bool IDBuf::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            id = getString(msg, keyId, 0);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void IDBuf::serializeToPBMsg(struct pbc_wmessage* msg) {
            setString(msg, keyId, id);
        }
    }
}
