//
//  message.hpp
//  proto
//
//  Created by WF Chat on 2017/12/1.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef load_remote_messages_hpp
#define load_remote_messages_hpp

#include <stdio.h>
#include <string>
#include "pbbase.h"
#include "conversation.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        
        class LoadRemoteMessages : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            LoadRemoteMessages() {}
            virtual ~LoadRemoteMessages() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            Conversation conversation;
            int64_t beforeUid;
            int count;
        };
    }
}
#endif /* load_remote_messages_hpp */

