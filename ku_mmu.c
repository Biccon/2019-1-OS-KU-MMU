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
  ku_mmu_init(40, 64);

  printf("process1 run success %d\n", ku_run_proc(1, &ku_cr3)); // -1 0 0 0 0 0 0 0
  show_page();
  printf("page fault success %d\n", ku_page_fault(1, 100)); // -1 -1 -1 1 0 0 0 0
  show_page();
  printf("page fault success %d\n", ku_page_fault(1, 96)); // -1 -1 -1 1 2 0 0 0
  show_page();
  printf("page fault success %d\n", ku_page_fault(1, 16)); // -1 -1 -1 1 2 -1 -1 3
  show_page();
  printf("page fault success %d\n", ku_page_fault(1, 20)); // -1 -1 -1 4 2 -1 -1 3 swap(pfn3 to sfn1) // sfn1 원래 100매치
  show_page();
  //////// process 2 run
  printf("process2 run success %d\n", ku_run_proc(2, &ku_cr3));  // -1 -1 -1 4 -1(pd) -1 -1 3 swap(pfn4 to sfn2), sfn2에 원래 96매치
  show_page();
  printf("page fault success %d\n", ku_page_fault(2, 100)); // -1 -1 -1
  show_page();
  printf("fault %d\n", ku_page_fault(1, 96)); // -1 -1 -1 4 -1 -1 -1 5 swap(pfn7 to sfn3), sfn3에 16매치, 96 swapin(sfn2 to pfn7)
  show_page();
  printf("fault %d\n", ku_page_fault(1, 16)); // -1 -1 -1 6 -1 -1 -1 5,swap(pfn3 to sfn2), sfn2에 20매치,  16 swapin(sfn3 to pfn 3), 
  show_page();
  printf("fault %d\n", ku_page_fault(1, 20)); // -1 -1 -1 6 -1 -1 -1 7, swap(pfn7 to sfn3), sfn3에 96매치, 20 swapin(sfn2 to pfn7)
  show_page();
  printf("fault %d\n", ku_page_fault(1, 16)); // -1 -1 -1 6 -1 -1 -1 7, no swap 
  show_page();
  printf("fault %d\n", ku_page_fault(1, 20)); // -1 -1 -1 6 -1 -1 -1 7, no swap
  show_page();
  printf("fault %d\n", ku_page_fault(1, 96)); // -1 -1 -1 4 -1 -1 -1 5 swap(pfn7 to sfn3), sfn3에 16매치, 96 swapin(sfn2 to pfn7)
  show_page();
  show_swap();
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