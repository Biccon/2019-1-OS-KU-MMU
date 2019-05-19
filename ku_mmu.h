#include <stdio.h>
#include <stdlib.h>
#include "ku_linkedlist.h"

typedef struct ku_pte
{
    unsigned char data;
    /*
    // page entry(pde/pte) - 8bits 짜리 ku_pte
    unsigned int pfn : 7; // redundant를 대체해서 <<1 하면 될 듯.
    //unsigned int redundant : 1; // 1비트 남음
    unsigned int present : 1; // present 0이면 스왑공간, offset이 swap의 offset(1부터 시작)
    */
} ku_pte;

typedef struct ku_h_page
{
    int swapable;
    struct ku_pte **pba; // page base address *ku_pte를 4개 가질 수 있는 페이지
} ku_h_page;

ku_h_page *ku_h_memory;
int *ku_h_swapspace;
ku_h_linkedlist *ku_h_processes;

unsigned int ku_h_mem_size; // 40 byte
unsigned int ku_h_swap_size;
unsigned int ku_h_page_size = 4; // 4byte
unsigned int ku_h_page_count;
unsigned int ku_h_swap_count;
unsigned int ku_h_page_index = 1; // 페이지 리스트에 할당된 순서 1부터 유의미하므로 1
unsigned int ku_h_swap_index = 1; // offset이 1부터 가능하므로 1

void set_ku_pte(struct ku_pte pte, char a, char b )
{
    pte.data = a << 1 | b;
}

int get_ku_pte_pfn(struct ku_pte pte)
{
    return pte.data & 0xFE;
}

int get_ku_pte_present(struct ku_pte pte)
{
    return pte.data & 1;
}

struct ku_pte **make_new_ptes()
{
    struct ku_pte **ptes = (struct ku_pte **)malloc(sizeof(struct ku_pte *) * ku_h_page_size);
    for (int i = 0; i < ku_h_page_size; i++)
    {
        ptes[i] = (struct ku_pte *)calloc(1, sizeof(struct ku_pte));
    }
    return ptes;
}

int get_count(int data)
{
    int count = 0;
    for (int i = 0; i < ku_h_page_count; i++)
    {
        if (ku_h_memory[i].swapable == data)
            count++;
    }
    return count;
}

int get_firstin_page()
{
    int min = __INT_MAX__;
    int min_index = -1;
    for (int i = 0; i < ku_h_page_count; i++)
    {
        int temp = ku_h_memory[i].swapable;
        if (temp < min && temp > 0)
        {
            min = temp;
            min_index = i;
        }
    }
    return min_index;
}

int get_page(char pid, char swappable)
{
    int pfn = -1;
    printf("------------------------------\n");
    printf("before free : %d, swap X : %d, swapable : %d\n", get_count(0), get_count(-1), ku_h_page_count - (get_count(0) + get_count(-1)));
    if (get_count(0) == 0)                    // 0은 할당안된 페이지
    {                                         // 모든 페이지 공간이 가득차있음
        if (get_count(-1) != ku_h_page_count) // -1이 swap불가능한 페이지
        {
            pfn = get_firstin_page(); // first in 된 페이지 구해서 first out~~
            //swap insert ku_h_memory[pfn].pba
            // 스왑이루어지면 됨, 스왑 된 결과값을 어떻게 주느냐가 문젠데..
            // ku_pte를 매개변수로 받아서, present 0으로 바꾸고, offset에 swapindex넣으면 될듯
            ku_h_memory[pfn].swapable = (swappable == -1) ? swappable : ku_h_page_index++; // 새로 할당해준것임.
            ku_h_memory[pfn].pba = NULL;
            // 값이 1이상인것 중에 minimum찾아서 주면 되겠당^^
        }
        else
        {
            printf("cannot swap anymore\n");
        }
    }
    else
    {
        for (int i = 0; i < ku_h_page_count; i++)
        {
            if (ku_h_memory[i].swapable == 0)
            {
                pfn = i;
                ku_h_memory[i].swapable = (swappable == -1) ? swappable : ku_h_page_index++;
                break;
            }
        }
    }
    printf("after free : %d, swap X : %d, swapable : %d\n", get_count(0), get_count(-1), ku_h_page_count - (get_count(0) + get_count(-1)));

    return pfn;
}

int ku_page_fault(char pid, char va)
{
    ku_h_node *process_node = ku_h_get_node_by_pid(ku_h_processes, pid);

    char offset_pd = (va & 0xC0) >> 6;  // page directory offset
    char offset_pmd = (va & 0x30) >> 4; // page middle directory offset
    char offset_pt = (va & 0x0C) >> 2;  // page table offset == VPN
    char offset = (va & 0x03);

    printf("%u %u %u %u\n", offset_pd, offset_pmd, offset_pt, offset);
    return 0;
}

void *ku_mmu_init(unsigned int mem_size, unsigned int swap_size)
{
    ku_h_mem_size = mem_size;
    ku_h_page_count = mem_size / ku_h_page_size;
    ku_h_swap_size = swap_size;
    ku_h_swap_count = swap_size / ku_h_page_size;

    ku_h_processes = (ku_h_linkedlist *)malloc(sizeof(ku_h_linkedlist));
    ku_h_memory = (ku_h_page *)calloc(ku_h_page_count, sizeof(ku_h_page)); // page개수만큼만 생각하자
    ku_h_swapspace = (int *)calloc(ku_h_swap_count, sizeof(int));          // offset 1부터 시작

    ku_h_init_list(ku_h_processes);

    return (void *)ku_h_memory;
}

int ku_run_proc(char pid, struct ku_pte **ku_cr3)
{
    ku_h_node *process = ku_h_get_node_by_pid(ku_h_processes, pid);
    if (process == NULL)
    {
        ku_h_page *page_for_pd = get_page(pid, -1);
        if (page_for_pd != -1)
        {
            // pd를 위한 페이지 얻었음
        }
    }
    return 0;
}

void print_page()
{
    for (int i = 0; i < ku_h_page_count; i++)
    {
        printf("%d ", ku_h_memory[i].swapable);
    }
    printf("\n");
}

void test(int testcase[], int count)
{
    for (int i = 0; i < count; i++)
    {
        printf("PFN : %d\n", get_page(0, testcase[i]));
        print_page();
    }
}
