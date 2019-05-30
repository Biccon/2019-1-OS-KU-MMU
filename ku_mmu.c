#include <stdio.h>
#include "ku_mmu.h"

int main(int argc, char **argv) {
  void *ku_cr3;
  ku_mmu_init(40, 64);

  printf("process1 run success %d\n", ku_run_proc(1, &ku_cr3)); // -1 0 0 0 0 0 0 0
  
  printf("page fault success %d\n", ku_page_fault(1, 100)); // -1 -1 -1 1 0 0 0 0
  
  printf("page fault success %d\n", ku_page_fault(1, 96)); // -1 -1 -1 1 2 0 0 0
  
  printf("page fault success %d\n", ku_page_fault(1, 16)); // -1 -1 -1 1 2 -1 -1 3
  
  printf("page fault success %d\n", ku_page_fault(1, 20)); // -1 -1 -1 4 2 -1 -1 3 swap(pfn3 to sfn1) // sfn1 원래 100매치
  
  //////// process 2 run
  printf("process2 run success %d\n", ku_run_proc(2, &ku_cr3));  // -1 -1 -1 4 -1(pd) -1 -1 3 swap(pfn4 to sfn2), sfn2에 원래 96매치
  
  printf("page fault success %d\n", ku_page_fault(2, 100)); // -1 -1 -1
  
  printf("fault %d\n", ku_page_fault(1, 96)); // -1 -1 -1 4 -1 -1 -1 5 swap(pfn7 to sfn3), sfn3에 16매치, 96 swapin(sfn2 to pfn7)
  
  printf("fault %d\n", ku_page_fault(1, 16)); // -1 -1 -1 6 -1 -1 -1 5,swap(pfn3 to sfn2), sfn2에 20매치,  16 swapin(sfn3 to pfn 3), 
  
  printf("fault %d\n", ku_page_fault(1, 20)); // -1 -1 -1 6 -1 -1 -1 7, swap(pfn7 to sfn3), sfn3에 96매치, 20 swapin(sfn2 to pfn7)
  
  printf("fault %d\n", ku_page_fault(1, 16)); // -1 -1 -1 6 -1 -1 -1 7, no swap 
  
  printf("fault %d\n", ku_page_fault(1, 20)); // -1 -1 -1 6 -1 -1 -1 7, no swap
  
  printf("fault %d\n", ku_page_fault(1, 96)); // -1 -1 -1 4 -1 -1 -1 5 swap(pfn7 to sfn3), sfn3에 16매치, 96 swapin(sfn2 to pfn7)
  
}