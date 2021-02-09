#include "client.h"

using namespace std;

int main()
{
    try
    {
        cout<<"Client start."<<endl;
        Client cl;
        cl.run();
    }
    catch (std::exception &e)
    {
        cout<<e.what()<<endl;
    }

    return 0;
}
