#include <iostream>
#include <sstream>
#include "server.h"

using namespace std;

int main()
{
    try {
        cout<<"server start."<<endl;
        Server srv;
        srv.run();
    }  catch (exception& e) {
        cout<<e.what()<<endl;
    }
    return 0;
}
