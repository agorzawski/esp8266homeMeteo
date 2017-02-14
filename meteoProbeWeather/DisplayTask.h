#include "task.hpp"
using namespace Tasks;

class DisplayTask : public Task
{
  public: 
    
    DisplayTask(int pin_sda, int pin_scl){   
    }
    
    virtual void run()
    {
        logPrintf("Will write something on the OLED screen :) ");
        sleep(1_s);
    }

    void printCurrentData()
    {
    }
    
  private:
    int _count = 1;

};    
