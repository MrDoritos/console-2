#pragma once
#include "console.h"

namespace cons {
    struct _auto : console {
        
        //Automatic construction upon file inclusion
        friend class constructor;
	
        struct constructor {
            constructor();
            ~constructor();
        };
        
        static constructor cons;	
    
    };

    
}

cons::_auto::constructor cons::_auto::cons;

cons::_auto::constructor::constructor() {	

}

cons::_auto::constructor::~constructor() {
    
}