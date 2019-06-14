//
//  id_buf.hpp
//  proto
//
//  Created by WF Chat on 2017/12/1.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef int64_buf_hpp
#define int64_buf_hpp

#include <stdio.h>
#include <string>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        
        class INT64Buf : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            INT64Buf() {}
            virtual ~INT64Buf() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            int64_t id;
        };
    }
}
#endif /* int64_buf_hpp */




