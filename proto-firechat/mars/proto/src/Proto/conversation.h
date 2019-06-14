//
//  conversation.hpp
//  proto
//
//  Created by WF Chat on 2017/11/29.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef conversation_hpp
#define conversation_hpp

#include <stdio.h>
#include <string>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        typedef enum {
            ConversationType_Private = 0,
            ConversationType_Group = 1,
            ConversationType_ChatRoom = 2,
            ConversationType_Channel = 2,
        } ConversationType;
        
        class Conversation : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            Conversation() {}
            virtual ~Conversation() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            void serializeToPBMsg(struct pbc_wmessage*);
            
            
            ConversationType type;
            std::string target;
            int line;
        };
    }
}
#endif /* conversation_hpp */
