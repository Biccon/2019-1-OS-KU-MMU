#include "ku_mmu.h"

int main(int argc, char **argv)
{
    //ku_page_fault(0, 0xA1); // 10(2) 10(2) 00 01(1)
    ku_mmu_init(32, 64);

    int testcase[] = {-1, 0, 0, -1, -1, 0, 0, 0, -1, -1, -1, -1, 1, 1, 1};
    test(testcase, sizeof(testcase) / sizeof(testcase[0]));
}
