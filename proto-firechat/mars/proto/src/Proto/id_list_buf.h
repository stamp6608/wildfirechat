//
//  id_list_buf.hpp
//  proto
//
//  Created by WF Chat on 2017/12/1.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef id_list_buf_hpp
#define id_list_buf_hpp

#include <stdio.h>
#include <string>
#include <list>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        
        class IDListBuf : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            IDListBuf() {}
            virtual ~IDListBuf() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::list<std::string> ids;
        };
    }
}
#endif /* add_friends_request_hpp */
