//
//  get_upload_token_result.cpp
//  proto
//
//  Created by WF Chat on 2017/12/1.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "get_upload_token_result.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* GetUploadTokenResult::getTypeName() {
            return "GetUploadTokenResult";
        }
        
        bool GetUploadTokenResult::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            domain = getString(msg, keyDomain, 0);
            token = getString(msg, keyToken, 0);
            server = getString(msg, keyServer, 0);
            port = getInt(msg, keyPort, 0);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void GetUploadTokenResult::serializeToPBMsg(struct pbc_wmessage* msg) {
            setString(msg, keyDomain, domain);
            setString(msg, keyToken, token);
            setString(msg, keyServer, server);
            setInt(msg, keyPort, port);
        }
    }
}
