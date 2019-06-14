//
//  message.hpp
//  proto
//
//  Created by WF Chat on 2017/12/1.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef message_hpp
#define message_hpp

#include <stdio.h>
#include <string>
#include "pbbase.h"
#include "conversation.h"
#include "messagecontent.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        
        class Message : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            Message() {}
            virtual ~Message() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            Conversation conversation;
            std::string fromUser;
            MessageContent content;
            int64_t messageId;
            int64_t serverTimestamp;
            std::list<std::string> tos;
        };
    }
}
#endif /* add_friends_request_hpp */

