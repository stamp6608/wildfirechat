//
//  modify_group_info_request.hpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef modify_channel_info_request_hpp
#define modify_channel_info_request_hpp

#include <stdio.h>
#include <string>
#include "pbbase.h"
#include "group.h"
#include <list>
#include "messagecontent.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;        
        
        class ModifyChannelInfo : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            ModifyChannelInfo() {}
            virtual ~ModifyChannelInfo() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::string channelId;
            int type;
            std::string value;
        };
    }
}
#endif /* modify_channel_info_request_hpp */

