//
//  user.hpp
//  proto
//
//  Created by WF Chat on 2017/12/1.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef search_user_request_hpp
#define search_user_request_hpp

#include <stdio.h>
#include <string>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
  namespace stn{
    class PBBase;
    
    
    class SearchUserRequest : public PBBase{
    protected:
      virtual const char* getTypeName();
    public:
      SearchUserRequest() {}
      virtual ~SearchUserRequest() {}
      virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
      virtual void serializeToPBMsg(struct pbc_wmessage* msg);
      
      std::string keyword;
      bool buzzy;
      int page;
    };
  }
}
#endif /* search_user_request_hpp */
