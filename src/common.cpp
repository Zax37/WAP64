#include "common.h"

using namespace std;

unsigned getFirstNumber(const char* s)
{
    string str = s;

    for(string::iterator c = str.begin(); c!=str.end(); c++)
        if(*c >= '0' && *c <= '9') { return atoi(&*c); }
    return 0;
}

