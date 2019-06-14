//
//  user.hpp
//  proto
//
//  Created by WF Chat on 2017/12/1.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef route_request_hpp
#define route_request_hpp

#include <stdio.h>
#include <string>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        
        class RouteRequest : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            RouteRequest() {}
            virtual ~RouteRequest() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::string app;
            int platform;
            int pushType;
            std::string deviceName;
            std::string deviceVersion;
            std::string phoneName;
            std::string language;
            std::string carrierName;
            std::string appversion;
            std::string sdkversion;
        };
    }
}
#endif /* route_request_hpp */
