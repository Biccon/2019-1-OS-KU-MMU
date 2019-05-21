#include <stdio.h>
#include <stdlib.h>
#include "ku_mmu.h"
#include "ku_trav.h"

int main(void){
	char pid=0, va, pa;
	unsigned int pmem_size = 32, swap_size = 64;
	void *ku_cr3, *pmem=NULL;
    char works[][2] = {
        {1, 100},
        {1, 10},
        {1, 12},
        {1, 8},
        {1, 20},
        {1, 10},
        {1, 12},
        {1, 8},
        {1, 20},
        {1, 10},
        {1, 12},
        {1, 8},
        {1, 20},
        {1, 10},
        {1, 12},
        {1, 8},
        {1, 20}
    };

	pmem = ku_mmu_init(pmem_size, swap_size);

    int workcount = sizeof(works)/sizeof(works[0]);

    for(int i=0;i<workcount;i++){
        char tpid = works[i][0];
        va = works[i][1];
        //printf("tpid %d va %d\n", tpid, va);
        if(pid != tpid) {
			if(ku_run_proc(tpid, &ku_cr3) == 0)
				pid = tpid; /* context switch */
            else {
                printf("Context change Failed\n");
                return 1;
            }
        }
        pa = ku_traverse(ku_cr3, va, pmem);
        if(pa == 0){
            if(ku_page_fault(pid, va) != 0){
                printf("Fault handler is failed\n");
                return 1;
            }
            printf("[%d] VA: %hhd -> Page Fault\n", pid, va);

            /* Retry after page fault */
            pa = ku_traverse(ku_cr3, va, pmem); 
            if(pa == 0){
                printf("ku_cpu: Addr tanslation is failed\n");
                return 1;
            }
        }
        printf("[%d] VA: %hhd -> PA: %hhd\n", pid, va, pa);
    }
}