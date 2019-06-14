//
//  user.hpp
//  proto
//
//  Created by WF Chat on 2017/12/1.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef im_http_wrapper_hpp
#define im_http_wrapper_hpp

#include <stdio.h>
#include <string>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
  namespace stn{
    class PBBase;
    
    
    class IMHttpWrapper : public PBBase{
    protected:
      virtual const char* getTypeName();
    public:
      IMHttpWrapper() {}
      virtual ~IMHttpWrapper() {}
      virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
      virtual void serializeToPBMsg(struct pbc_wmessage* msg);
      
      std::string token;
      std::string clientId;
      std::string request;
      std::string data;
    };
  }
}
#endif /* im_http_wrapper_hpp */
