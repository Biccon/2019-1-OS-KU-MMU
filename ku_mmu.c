#include <stdio.h>
#include <stdlib.h>
#include "ku_queue.h"

struct ku_pte { // page entry(pde/pte) - 8bits 짜리 ku_pte
    unsigned int pfn : 7; // redundant를 대체해서 <<1 하면 될 듯.
    //unsigned int redundant : 1; // 1비트 남음
    unsigned int present : 1;
};

struct ku_pcb {
    char pid; // process pid
    char pdba; // page directory base address
};

unsigned char *ku_h_memory;
unsigned char *ku_h_swapspace;
struct ku_pcb** ku_h_processes;

unsigned int ku_h_mem_size; // 40 byte
unsigned int ku_h_swap_size;
unsigned int ku_h_page_size = 4; // 4byte

struct Queue *pagelist;
struct Queue *swap;

void ku_h_init_queue(){
    pagelist = (struct Queue*)malloc(sizeof(struct Queue));
    pagelist -> front = pagelist -> rear = NULL;
    swap = (struct Queue*)malloc(sizeof(struct Queue));
    swap -> front = swap -> rear = NULL;
}

int ku_h_get_page(){
    // 만약 pfn 다 사용중이라면 FIFO로 스왑
    // 
    if(isFull() == 1) {
        int swapPFN = deQueue();
        swapEnqueue(swapPFN);
        return swapPFN;
    } else {
        
    }
}

int ku_page_fault(char pid, char va){
    char offset_pd = (va & 0xC0) >> 6; // page directory offset
    char offset_pmd = (va & 0x30) >> 4; // page middle directory offset
    char offset_pt = (va & 0x0C) >> 2; // page table offset == VPN
    char offset = (va & 0x03);

    printf("%u %u %u %u\n", offset_pd, offset_pmd, offset_pt, offset);
    /*
        Handling a page fault caused by on-demand paging or swapping
        • Page replacement policy: FIFO
        – pid: process id
        – va: virtual address
        – Return value
        • 0: success
        • -1: fail
    */
    return 0;
}

int ku_mmu_init(unsigned int mem_size, unsigned int swap_size){
    ku_h_init_queue();
    ku_h_mem_size = mem_size;
    ku_h_swap_size = swap_size;
    
    ku_h_processes = (struct ku_pcb*)malloc(sizeof(struct ku_pcb) * 256); // 128 만듦(>0인 char범위가 127개)
    ku_h_memory = (unsigned char*)calloc(mem_size, sizeof(char)); // mem_size Byte만큼 memory 있음
    ku_h_swapspace = (unsigned char*)calloc(swap_size, sizeof(char)); // offset 1부터 시작
    
    ku_h_memory[0] = 0xff;
    ku_h_memory[1] = 0xf0;
    ku_h_memory[2] = 0x04;
    ku_h_memory[3] = 0x10;
    
    if(ku_h_memory == NULL || ku_h_memory == NULL) // 메모리 할당 실패하면 fail
        return -1;
    return 0;
    /* 
        Resource initialization function
            • Will be called only once at the initialization phase
        – mem_size: physical memory size in bytes
            • Do consider the memory space consumed by page directories and tables
            • You don’t need to allocate real memory space, but simulate its behavior
        – swap_size: swap disk size in bytes
            • You don’t need to allocate real disk space, but simulate its behavior
        – Return value
            • 0: success
            • -1: fail
    */
}

int ku_run_proc(char pid, struct ku_pte *ku_cr3) {
    struct ku_pcb* pcb = (ku_h_processes + pid);
    if(pcb == NULL) {
        
    }
    /* 
        Creates a process or performs a context switch
        – pid: pid of the next process
        – ku_cr3: base address of the page directory for the next process
            • Points an 8-bit PDE/PTE
            • Its value should be changed appropriately by this function
        – Return value
            • 0: success
            • -1: fail
    */

    return 0;
}

void ku_page_print(unsigned int pfn) {
    unsigned char* page = &ku_h_memory[pfn * 4];
    printf("PFN %d : ", pfn);
    for(int index=0;index<ku_h_page_size;index++){
        printf("%02x ", page[index]);
    }
    printf("\n");
}

int ku_print_memory() {
    unsigned int max_pfn = ku_h_mem_size / ku_h_page_size;
    for(int pfn=0;pfn<max_pfn;pfn++){
        ku_page_print(pfn);
    }
}

int main(int argc, char** argv){
    //ku_page_fault(0, 0xA1); // 10(2) 10(2) 00 01(1)
    ku_mmu_init(12, 40);
    ku_print_memory();
}