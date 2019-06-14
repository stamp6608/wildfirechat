//
//  add_friends_request.hpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef search_channel_result_hpp
#define search_channel_result_hpp

#include <stdio.h>
#include <string>
#include <list>
#include "pbbase.h"
#include "channel_info.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
      
        class SearchChannelResult : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            SearchChannelResult() {}
            virtual ~SearchChannelResult() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::list<ChannelInfo> channels;
            std::string    keyword;
        };
    }
}
#endif /* search_channel_result_hpp */
