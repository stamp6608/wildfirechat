//
//  user.cpp
//  proto
//
//  Created by WF Chat on 2017/12/1.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "search_user_request.h"
#include "pbc/pbc.h"

namespace mars{
  namespace stn{
    
    const char* SearchUserRequest::getTypeName() {
      return "SearchUserRequest";
    }
    
    bool SearchUserRequest::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
      keyword = getString(msg, keyKeyword, 0);
      buzzy = getInt(msg, keyFuzzy, 0) > 0;
      page = getInt(msg, keyPage, 0);
      
      if(destroy)
        finishRead(msg);
      return true;
    }
    
    void SearchUserRequest::serializeToPBMsg(struct pbc_wmessage* msg) {
      setString(msg, keyKeyword, keyword);
      if (buzzy) {
        setInt(msg, keyFuzzy, 1);
      }
      
      if(page > 0) {
        setInt(msg, keyPage, page);
      }
    }
  }
}

