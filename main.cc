#include <iostream>
#include <thread>
#include <functional>
#include "include/timerpoll.h"

using namespace std;

// timers times1;
int Print(){
    cout << "print func" << endl;
    return 0;
}

class Test{
public:
    Test(int a, int b):_a(a), _b(b){}
    int Print(int aa, int bb){
        cout << aa - bb << endl;
        cout << _a + _b << endl;
        cout << "class print func" << endl;
    }
private:
    int _a;
    int _b;
};


int Add(int a, int b){
    cout << a + b << endl;
    cout << "add func" << endl;
    return 0;
}


int main()
{

    timers times1;
    times1.Init();
   
    times1.AddTimer(2.2, std::bind(Add,20, 50), (void*)"hello 2.2", 0);
 
    times1.AddTimer(3.3, Print, (void*)"hello Print3.3", 0);

    Test *t1 = new Test(5, 10);
    times1.AddTimer(3.2,std::bind(&Test::Print, t1, 100, 50), NULL, (void*)"helloclass 0.5", 0);

    Test t2(10, 20);
    times1.AddTimer(4.4, std::bind(&Test::Print, &t2, 20, 50), NULL, (void*)"helloclass 4.4", 0);

    cout << "timer num: "<< times1.Size() << endl;

    times1.Stop();

    return 0;
}