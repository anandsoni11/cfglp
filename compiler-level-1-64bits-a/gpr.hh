#ifndef GPR_HH
#define GPR_HH

#define NUM_REGS 16

#include <string>
#include <map>
using namespace std;

typedef enum
{
    zero = 0,   //zero
    at = 1,     //assembler temporary
    v0 = 2,     //value
    v1 = 3,
    a0 = 4,     //arguments
    a1 = 5,
    a2 = 6,
    a3 = 7,
    t0 = 8,     //temporaries
    t1 = 9,
    t2 = 10,
    t3 = 11,
    t4 = 12,
    t5 = 13,
    t6 = 14,
    t7 = 15
} Register_Enum;

static map<int, Register_Enum> int_reg_enum_map = 
{
    {0 , zero},
    {1 , at },
    {2 , v0 },
    {3 , v1 },
    {4 , a0 },
    {5 , a1 },
    {6 , a2 },
    {7 , a3 },
    {8 , t0 },
    {9 , t1 },
    {10 , t2 },
    {11 , t3 },
    {12 , t4 },
    {13 , t5 },
    {14 , t6 },
    {15 , t7 }
};

static map<Register_Enum, string> reg_str_map = 
{
    {zero ,"zero"},
    {at ,"at"},
    {v0 ,"v0"},
    {v1 ,"v1"},
    {a0 ,"a0"},
    {a1 ,"a1"},
    {a2 ,"a2"},
    {a3 ,"a3"},
    {t0 ,"t0"},
    {t1 ,"t1"},
    {t2 ,"t2"},
    {t3 ,"t3"},
    {t4 ,"t4"},
    {t5 ,"t5"},
    {t6 ,"t6"},
    {t7 ,"t7"}
};    

class GPR{
    bool register_status[NUM_REGS]; //whether used(1) / free(0)
public:
    GPR();//init the map to all 0
    ~GPR();
    Register_Enum get_first_icode_free(); //For icode return Register_Enum & also sets its status to USED
                                            //Consider registers v0, t0-t7
    void free(Register_Enum reg); //Sets the status of reg to FREE
    void reset();   //reset register status map
};


extern GPR cpu_gpr;
#endif
