//
//  add_friends_request.hpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef route_response_hpp
#define route_response_hpp

#include <stdio.h>
#include <string>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        
        class RouteResponse : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            RouteResponse() {}
            virtual ~RouteResponse() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::string host;
            int longPort;
            int shortPort;
        };
    }
}
#endif /* route_response_hpp */
