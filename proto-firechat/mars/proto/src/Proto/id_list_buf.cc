//
//  id_list_buf.cpp
//  proto
//
//  Created by WF Chat on 2017/12/1.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "id_list_buf.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* IDListBuf::getTypeName() {
            return "IDListBuf";
        }
        
        bool IDListBuf::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            for (int i = 0; i < getSize(msg, keyId); i++) {
                std::string value = getString(msg, keyId, i);
                ids.push_back(value);
            }
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void IDListBuf::serializeToPBMsg(struct pbc_wmessage* msg) {
            for (std::list<std::string>::iterator it = ids.begin(); it != ids.end(); it++) {
                setString(msg, keyId, *it);
            }
        }
    }
}
