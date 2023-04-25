#include <stdio.h>
#include <unistd.h>

int calculate()
{
    int i = 0, ret;
    for (i = 0; i < 10000000; i++)
    {
        ret += i;
    }
    return ret;
}

int main(int argc, char **argv)
{
    int loop;
    for (loop = 0; loop < 5000; loop++)
    {
        calculate();
    }
}
