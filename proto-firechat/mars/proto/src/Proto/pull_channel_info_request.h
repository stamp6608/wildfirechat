//
//  pull_group_member.hpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef pull_channel_info_hpp
#define pull_channel_info_hpp

#include <stdio.h>
#include <string>
#include <list>
#include "pbbase.h"
#include "group.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        
        class PullChannelInfoRequest : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            PullChannelInfoRequest() {}
            virtual ~PullChannelInfoRequest() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::string channelId;
            int64_t head;
        };
    }
}
#endif /* pull_channel_info_hpp */

