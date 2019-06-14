//
//  modify_my_info_request.hpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef modify_my_info_request_hpp
#define modify_my_info_request_hpp

#include <stdio.h>
#include <string>
#include <list>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;

        class InfoEntry : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            InfoEntry() {}
            virtual ~InfoEntry() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            int type;
            std::string value;
        };

        
        class ModifyMyInfoRequest : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            ModifyMyInfoRequest() {}
            virtual ~ModifyMyInfoRequest() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::list<InfoEntry> entrys;
        };
    }
}
#endif /* add_friends_request_hpp */

