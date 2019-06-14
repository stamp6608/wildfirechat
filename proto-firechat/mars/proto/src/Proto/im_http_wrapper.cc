//
//  user.cpp
//  proto
//
//  Created by WF Chat on 2017/12/1.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "im_http_wrapper.h"
#include "pbc/pbc.h"

namespace mars{
  namespace stn{
    
    const char* IMHttpWrapper::getTypeName() {
      return "IMHttpWrapper";
    }
    
    bool IMHttpWrapper::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
      token = getString(msg, keyToken, 0);
      data = getString(msg, keyData, 0);
      request = getString(msg, keyRequest, 0);
      clientId = getString(msg, keyClientId, 0);
      
      if(destroy)
        finishRead(msg);
      return true;
    }
    
    void IMHttpWrapper::serializeToPBMsg(struct pbc_wmessage* msg) {
      setString(msg, keyToken, token);
      setString(msg, keyClientId, clientId);
      setString(msg, keyRequest, request);
      setString(msg, keyData, data);
    }
  }
}

