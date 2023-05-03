#include <stdio.h>
#include <unistd.h>

int calculate()
{
    int i = 0, ret;
    for (i = 0; i < 1000000; i++)
    {
        ret += i;
    }
    return ret;
}

int main(int argc, char **argv)
{
    int loop;
    for (loop = 0; loop < 100; loop++)
    {
        printf("%d\n", loop);
        sleep(1);
        calculate();
    }
}
