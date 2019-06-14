//
//  transfer_group_request.hpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef transfer_channel_request_hpp
#define transfer_channel_request_hpp

#include <stdio.h>
#include <string>
#include <list>
#include "pbbase.h"
#include "messagecontent.h"


struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        class TransferChannelRequest : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            TransferChannelRequest() {}
            virtual ~TransferChannelRequest() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::string channelId;
            std::string newOwner;
        };
    }
}
#endif /* transfer_channel_request_hpp */


