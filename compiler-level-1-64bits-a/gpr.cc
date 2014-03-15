#include "gpr.hh"
GPR::GPR(){
    reset();
}

GPR::~GPR(){
}

void GPR::reset(){
    for(int i=0;i<NUM_REGS;i++){
        register_status[i] = false;
    }
}

void GPR::free(Register_Enum reg){
    register_status[reg] = false;
}

Register_Enum GPR::get_first_icode_free(){
    if(register_status[v0] == false){
        register_status[v0] = true;
        return v0;
    }
    for(int i= t0; i<= t7; i++){ //iterate over temporaries
        if(register_status[i] == false){
            register_status[i] = true;
            return int_reg_enum_map[i];
        }
    }
}
