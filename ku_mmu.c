#include "ku_mmu.h"

void test(int testcase[], int count) {
  for (int i = 0; i < count; i++) {
    printf("PFN : %d\n", get_page(testcase[i]));
    print_page();
  }
}

unsigned char test2(char pfn, char present) {
  char test = pfn << 1 | present;
  return test;
}
int main(int argc, char **argv) {
  void *ku_cr3;
  // ku_page_fault(0, 0xA1); // 10(2) 10(2) 00 01(1)
  ku_mmu_init(32, 64);

  printf("process1 run success %d\n", ku_run_proc(1, &ku_cr3));
  printf("process returned ku_cr3 %p\n", ku_cr3);
  page_travel(ku_cr3, 0);
  printf("page fault success %d\n", ku_page_fault(1, 100));
  page_travel(ku_cr3, 0);
  //////// process 2 run
  printf("process2 run success %d\n", ku_run_proc(2, &ku_cr3));
  printf("process returned ku_cr3 %p\n", ku_cr3);
  page_travel(ku_cr3, 0);
  printf("page fault success %d\n", ku_page_fault(2, 100));
  page_travel(ku_cr3, 0);
  /*
  ku_page_fault(1, 100); // -1 -1 -1 1 0 0 0 0
  print_page();  // pm 생성 한개
  print_all_page_entries();
  printf("fault %d\n", ku_page_fault(1, 96)); // -1 -1 -1 1 2 0 0 0
  print_page();  // pm 생성 한개
  print_all_page_entries();
  printf("fault %d\n", ku_page_fault(1, 16)); // -1 -1 -1 1 2 -1(pmd) -1(pt) 3(page)
  print_page();  // pm 생성 한개
  print_all_page_entries();
  printf("fault %d\n", ku_page_fault(1, 20)); // -1 -1 -1 4(swap) 2 -1 -1 3
  print_page();  // pm 생성 한개
  print_all_page_entries();
  printf("run success? %d\n", ku_run_proc(2, &ku_cr3)); // -1 -1 -1 4 -1(pd) -1 -1 3
  print_page();  // pm 생성 한개
  print_all_page_entries();
  printf("다시 1프로세스로 돌아와보자구\n");
  ku_page_fault(1, 96); // -1 -1 -1 
  print_page();  // pm 생성 한개
  print_all_page_entries();
  printf("fault %d\n", ku_page_fault(1, 16)); // -1 -1 -1 
  print_page();  // pm 생성 한개
  print_all_page_entries();
*/
  exit(-1);
  printf("------rerun\n");
  printf("fault %d\n", ku_page_fault(1, 96));
  printf("fault %d\n", ku_page_fault(1, 16));
  printf("fault %d\n", ku_page_fault(1, 20));
  print_page();  // pm 생성 한개

  
                 // printf("fault %d\n", ku_page_fault(2, 100));
  // print_page();
}
