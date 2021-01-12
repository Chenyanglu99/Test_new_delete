#include "SmartPtr.h"
class parent {};
class child : parent {};
int main(void)
{
    int i = 9;
    SmartPtr<int> pi(&i);

    SmartPtr<int> pd(pi);

    std::cout << pd.get();

    SmartPtr<parent> pt1 = SmartPtr<child>(new child);
    //SmartPtr<child> pt2 = SmartPtr<parent>(new parent);

    SmartPtr<const parent> pt3 = pt1;
}