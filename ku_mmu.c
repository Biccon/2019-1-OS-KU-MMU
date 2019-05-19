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

    printf("run success? %d\n", ku_run_proc(1, &ku_cr3));
    print_page(); // pm 생성 한개

    ku_h_print_list(ku_h_processes);

    ku_page_fault(1, 100);
    print_page();
    ku_page_fault(1, 96);
    print_page();
    ku_page_fault(1, 16);
    print_page();
    ku_page_fault(1, 20);
    print_page();
    printf("run success? %d\n", ku_run_proc(2, &ku_cr3));
    print_page(); // pm 생성 한개

    int testcase[] = {-1, 0, 0, -1, -1, 0, 0, 0, -1, -1, -1, -1, 1, 1, 1};
    
    int pfn = 3; // 0011
    printf("%d\n", test2(7, 0)); // 0111
    //test(testcase, sizeof(testcase) / sizeof(testcase[0]));
}
