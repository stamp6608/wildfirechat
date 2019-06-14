//
//  general_result.cpp
//  proto
//
//  Created by WF Chat on 2017/12/1.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "general_result.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* GeneralResult::getTypeName() {
            return "GeneralResult";
        }
        
        bool GeneralResult::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            errorCode = getInt(msg, keyErrorCode, 0);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void GeneralResult::serializeToPBMsg(struct pbc_wmessage* msg) {
            setInt(msg, keyErrorCode, errorCode);
        }
    }
}
