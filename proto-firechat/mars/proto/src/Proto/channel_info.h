//
//  group.hpp
//  proto
//
//  Created by WF Chat on 2017/11/29.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef channel_info_hpp
#define channel_info_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        class ChannelInfo : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            ChannelInfo() {}
            virtual ~ChannelInfo() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::string targetId;
            std::string name;
            std::string portrait;
            std::string owner;
            //0, public; 1, private; 2 closed;
            int status;
            std::string desc;
            std::string extra;
            std::string secret;
            std::string callback;
            int automatic;
            int64_t updateDt;
        };
    }
}
#endif /* channel_info_hpp */
