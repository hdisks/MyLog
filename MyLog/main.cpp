#include <cstdio>
#include<string>
#include<string.h>
#include<strings.h>
#include"Log.h"


using namespace std;
int main()
{
   int  m_close_log = 0;
    Log::getInstance()->init("./test/ServerLog.txt", 800, 0, 8000, 4096);
    LOG_INFO("deal with the client()", "fa");
    
    return 0;
}