#include "findstr.h"




FindStr::FindStr()
{
    
}

FindStr::config(unsigned char *str,int len)
{
    this->str = str;
    this->len = len;
}

FindStr::reset()
{
    index=0;
}

int FindStr::input(unsigned char *data,int len)
{
    for(int i=0;i<len;i++)
    {
        if(data[i] == str[index])
        {
            index++;
            if(index >= len)
            {
                return i+1;
            }
        }else{
            index = 0;
            if(data[i] == str[index])
            {
                index++;
                if(index >= len)
                {
                    return i+1;
                }
            }
        }
    }
    
    return 0;
}
