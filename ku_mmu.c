#include "ku_mmu.h"


void test(int testcase[], int count)
{
    for (int i = 0; i < count; i++)
    {
        printf("PFN : %d\n", get_page(testcase[i]));
        print_page();
    }
}

unsigned char test2(char pfn, char present) {
    char test = pfn << 1 | present;
    return test;
}
int main(int argc, char **argv)
{
    void *ku_cr3;
    //ku_page_fault(0, 0xA1); // 10(2) 10(2) 00 01(1)
    ku_mmu_init(32, 64);

    printf("run success? %d\n", ku_run_proc(1, (ku_pte**)&ku_cr3));
    printf("fault %d\n", ku_page_fault(1, 100));

    /*
    print_page(); // pm 생성 한개

    ku_h_print_list(ku_h_processes);

    printf("fault %d\n", ku_page_fault(1, 96));
    printf("fault %d\n", ku_page_fault(1, 16));
    printf("fault %d\n", ku_page_fault(1, 20));
    printf("run success? %d\n", ku_run_proc(2, &ku_cr3));
    print_page(); // pm 생성 한개
    printf("fault %d\n", ku_page_fault(2, 100));
    print_page();
    */
}
