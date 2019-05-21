#include <stdio.h>
#include <stdlib.h>
#include "ku_linkedlist.h"

typedef struct ku_pte {
  unsigned char data;
} ku_pte;

ku_pte* ku_h_memory;
int* ku_h_memory_swapable;
int* ku_h_swapspace;
ku_h_linkedlist* ku_h_processes;

unsigned int ku_h_mem_size;  // 40 byte
unsigned int ku_h_swap_size;
unsigned int ku_h_page_size = 4;  // 4byte
unsigned int ku_h_page_count;
unsigned int ku_h_swap_count;
unsigned int ku_h_page_index =
    1;  // 페이지 리스트에 할당된 순서 1부터 유의미하므로 1
unsigned int ku_h_swap_index = 1;  // offset이 1부터 가능하므로 1

void print_page() {
  for (int i = 0; i < ku_h_page_count; i++) {
    printf("%d ", ku_h_memory_swapable[i]);
  }
  printf("\n");
}

void set_ku_pte(struct ku_pte* pte, char a, char b) {
  pte->data = a | b;
}

void set_ku_pte_pfn(struct ku_pte* pte, char pfn) {
  set_ku_pte(pte, pfn << 2, 1);  // pfn은 present 1
}

void set_ku_pte_swap_offset(struct ku_pte* pte, char swap_offset) {
  set_ku_pte(pte, swap_offset << 1, 0);  // 스왑에는 present 0,
}

unsigned char get_ku_pte_pfn(struct ku_pte* pte) {
  return (pte->data) >> 2;  // pfn은 6비트이므로
}

unsigned char get_ku_pte_swap_offset(struct ku_pte* pte) {
  return (pte->data) >> 1;  // swap offset은 7비트이므로
}

unsigned char get_ku_pte_present(struct ku_pte* pte) { return pte->data & 0x1; }

struct ku_pte* make_new_ptes() {
  struct ku_pte* ptes =
      (struct ku_pte*)calloc(ku_h_page_size, sizeof(struct ku_pte));
  return ptes;
}

int get_count(int data) {
  int count = 0;
  for (int i = 0; i < ku_h_page_count; i++) {
    if (ku_h_memory_swapable[i] == data) count++;
  }
  return count;
}

int get_firstin_page() {
  int min = __INT_MAX__;
  int min_index = -1;
  for (int i = 0; i < ku_h_page_count; i++) {
    int temp = ku_h_memory_swapable[i];
    if (temp < min && temp > 0) {
      min = temp;
      min_index = i;
    }
  }
  return min_index;
}

int get_page(char swappable) {
  int pfn = -1;
  if (get_count(0) == 0)  // 0은 할당안된 페이지
  {                       // 모든 페이지 공간이 가득차있음
    if (get_count(-1) != ku_h_page_count)  // -1이 swap불가능한 페이지
    {
      pfn = get_firstin_page();
      ku_h_memory_swapable[pfn] =
          (swappable == -1) ? swappable
                            : ku_h_page_index++;  // 새로 할당해준것임.

                            /* 스왑공간 차례대로 넣어주는거 만들어야함. */
      int sfn = 0;
      for(int i=1;i<ku_h_swap_count;i++){ // swap offset은 1부터 가능임
        if(ku_h_swapspace[i] == 0){
          sfn = i;
          ku_h_swapspace[i] = ku_h_swap_index++; // 들어온순서
          //printf("pfn %d to sfn %d\n", pfn, sfn);
          break;
        }
      }
      int temp_index_of_pt = ku_h_memory[pfn*4].data;
      set_ku_pte_swap_offset(ku_h_memory+temp_index_of_pt, sfn); // 스왑해준다(pfn*4에 pt가 들어있으니까 일단 포인터 이용하고나서 초기화시켜야함)
      ku_h_memory[pfn*4].data = 0;// pt넣어줘야할건데? 스왑 받은곳에서 넣어주기로 했다
      ku_h_memory[pfn*4+1].data = 0;
      ku_h_memory[pfn*4+2].data = 0;
      ku_h_memory[pfn*4+3].data = 0;
      // FIFO해서 가장먼저들어온페이지를 스왑공간에 넣어주는거
      //printf("swap occured pfn %d %d\n", pfn, swappable);
    } else {
      //printf("cannot swap anymore\n");
    }
  } else {
    for (int i = 0; i < ku_h_page_count; i++) {
      if (ku_h_memory_swapable[i] == 0) {
        pfn = i;
        ku_h_memory_swapable[i] =
            (swappable == -1) ? swappable : ku_h_page_index++;
        break;
      }
    }
  }
  // printf("after free : %d, swap X : %d, swapable : %d\n", get_count(0),
  // get_count(-1), ku_h_page_count - (get_count(0) + get_count(-1)));

  return pfn;
}

int ku_page_fault(char pid, char va) {
  ku_h_node* process = ku_h_get_node_by_pid(ku_h_processes, pid);
  ku_h_pcb* pcb = process->pcb;

  char offset_pd = (va & 0xC0) >> 6;   // page directory offset 11000000
  char offset_pmd = (va & 0x30) >> 4;  // page middle directory offset 00110000
  char offset_pt = (va & 0x0C) >> 2;   // page table offset == VPN 00001100
  char offset = (va & 0x03);           // 00000011
  // printf("%u %u %u %u\n", offset_pd, offset_pmd, offset_pt, offset);
  // travel 시작~~
  ku_pte* pd =ku_h_memory + pcb->pdba + offset_pd;  // 처음 pd는 pdba로 접근합니다. pcb가 주소를 가지기 때문에.

  int pfn_of_pmd = -1;
  // printf("pd에 pmd가 매핑되어있나요?\n");
  if (pd->data == 0) {  // pd가 가리키는 pmd의 페이지가 할당되지 않음
    pfn_of_pmd = get_page(-1);  // unswapable
    if (pfn_of_pmd == -1) {
      return -1;
    } else {
      set_ku_pte_pfn(
          pd,
          pfn_of_pmd);  // present 1로 바꾸고, 할당된 pfn으로 pd의 pte에 넣어줌
      
      //pmd의 데이터를 할당함니다
      ku_h_memory[pfn_of_pmd*4].data = 0;// 새로운 pte넣어주는게 결국 data에 0만 넣어주는거랑 같음
      ku_h_memory[pfn_of_pmd*4+1].data = 0;
      ku_h_memory[pfn_of_pmd*4+2].data = 0;
      ku_h_memory[pfn_of_pmd*4+3].data = 0;
      
      //첫 접근이니까 ptes도 만들어줍니다.(페이지 할당 받았으니까~)
    }
  } else {
    char present_of_pd = get_ku_pte_present(pd);
    if (present_of_pd == 0) {
      // 스왑되어있음, 그런데 pmd는 스왑되지 않으므로 구현 안해도 됨
    } else {
      pfn_of_pmd = get_ku_pte_pfn(pd);
      // pfn에 할당되어 있음, 어차피 pmd 할당 안되어있어도 pmd뒤져줘야하니까
      // if문 밖에서 구현하면 중복 줄임 넘어가도 됨. 나중에 pd->data ==0
      // else부분 다 지우자.
    }
  }

  // printf("pmd에 pt가 매핑되어있나요?\n");
  ku_pte* pmd = (ku_h_memory + pfn_of_pmd*4) + offset_pmd;  // pmd 접근할 때는 ku_h_memory+pfn*4를 통해 얻어놓은 pfn을통해 접근합니다.
  int pfn_of_pt = -1;
  if (pmd->data == 0) {  // 위에서 마찬가지로 pmd에 대해 pt 페이지가 할당됐나
    // 확인합니다. 할당되지 않은 조건이므로 할당이 필요함.
    pfn_of_pt = get_page(-1);  // pt를 위한 페이지 할당
    if (pfn_of_pt == -1) {
      return -1;  // 할당 실패했으므로 오류
    } else {
      set_ku_pte_pfn(pmd, pfn_of_pt);

      ku_h_memory[pfn_of_pt*4].data = 0;
      ku_h_memory[pfn_of_pt*4+1].data = 0;
      ku_h_memory[pfn_of_pt*4+2].data= 0;
      ku_h_memory[pfn_of_pt*4+3].data = 0;
    }
  } else {
    char present_of_pmd = get_ku_pte_present(pmd);
    if (present_of_pmd == 0) {
      // PT가 마찬가지로 스왑되어있으나, 현재 스왑되지 않는다는 설정이 있으므로
      // 구현 안함
    } else {
      pfn_of_pt = get_ku_pte_pfn(pmd);
    }
  }

  // printf("pt에 page가 매핑되어있나요?\n");
  ku_pte* pt = (ku_h_memory + pfn_of_pt*4) + offset_pt; // 사실 pte였다
  int pfn_of_page = -1;
  if (pt->data == 0) { 
    pfn_of_page = get_page(0);
    if (pfn_of_page == -1) {
      return -1; 
    } else {
      set_ku_pte_pfn(pt, pfn_of_page);
      ku_h_memory[pfn_of_page*4].data = pfn_of_pt*4+offset_pt; // pt를 새롭게 찾을 방법이 필요함. 실제 pt의 페이지*4+그 pt page안에서의 offset을 넣어놓으면 거기로 직접 접근 가능
      ku_h_memory[pfn_of_page*4+1].data = 0;
      ku_h_memory[pfn_of_page*4+2].data = 0;
      ku_h_memory[pfn_of_page*4+3].data = 0;
    }
  } else {
    char present_of_page = get_ku_pte_present(pt);  // 현재 페이지가 스왑됐는지 확인
    if (present_of_page == 0) {
      pfn_of_page = get_page(0);  // 페이지 얻어와본다. 근데 이 페이지 주인은 어케아노 시발련ㄴ아? 페이지 주인은 ku_h_memory[pfn_of_page].pdb에 들어있다링
      
      if (pfn_of_page == -1) {
        return -1;
      } else {
        ku_h_swapspace[get_ku_pte_swap_offset(pt)] =
            0;  // 스왑스페이스에서 가져왔다고 표시해줌
                // 꺼내온것만 표시해주면 되고 나머지는 필요없을듯.
        set_ku_pte_pfn(
            pt,
            pfn_of_page);  // 스왑받은 페이지를 다시 pt에 pfn에 넣어줭
        ku_h_memory[pfn_of_page*4].data = pfn_of_pt*4+offset_pt; // pt를 새롭게 찾을 방법이 필요함. 실제 pt의 페이지*4+그 pt page안에서의 offset을 넣어놓으면 거기로 직접 접근 가능
      }
    } else {
      // 매핑된 페이지에 내용은 중요하지 않은 값.
    }
  }

  // printf("%u %u %u %u\n", offset_pd, offset_pmd, offset_pt, offset);
  return 0;
}

void* ku_mmu_init(unsigned int mem_size, unsigned int swap_size) {
  ku_h_mem_size = mem_size;
  ku_h_page_count = mem_size / ku_h_page_size;
  ku_h_swap_size = swap_size;
  ku_h_swap_count = swap_size / ku_h_page_size;

  ku_h_processes = (ku_h_linkedlist*)malloc(sizeof(ku_h_linkedlist));
  ku_h_memory = (ku_pte*)calloc(mem_size, sizeof(ku_pte));  // page개수만큼만 생각하자
  ku_h_memory_swapable =
      (int*)calloc(ku_h_page_count, sizeof(int));  // page개수만큼만 생각하자
  ku_h_swapspace =
      (int*)calloc(ku_h_swap_count, sizeof(int));  // offset 1부터 시작

  ku_h_init_list(ku_h_processes);

  return (void*)ku_h_memory;
}

int ku_run_proc(char pid, struct ku_pte** ku_cr3) {
  ku_h_node* process = ku_h_get_node_by_pid(ku_h_processes, pid);
  if (process == NULL) {
    int pfn_for_pd = get_page(
        -1);  // 최소한 Page Directory를 저장하기 위한 페이지가 한 개 필요하다.
    if (pfn_for_pd == -1) {
      return -1;  // page directory를 위한 page 할당이 불가능하므로 실행이
                  // 불가능하다. 오류 케이스 1
    } else {
      // 페이지 얻을 수 있으면 프로세스 만들 수 있으므로, ku_h_processes에
      // 프로세스를 하나 추가해주고, 추가된 리스트에 pcb를 하나 만들어서
      // 할당한다. pcb에 들어갈 pdba(실제 페이지의 pte 4개도 만들어서 할당한다)
      process = ku_h_add_last(ku_h_processes, pid);
      process->pcb = ku_h_make_pcb(pid);
      process->pcb->pdba = pfn_for_pd * 4;
    }
  }
  *ku_cr3 = (struct ku_pte*)(ku_h_memory+process->pcb->pdba);  // context change
  return 0;
}

void show_page(){
  printf("PFN\tswapable  [00]   [01]   [10]   [11] \n");
  for(int i=0;i<ku_h_mem_size;i++){
    if(i%4 == 0){
      printf("PFN %d\t%d\t", i/4, ku_h_memory_swapable[i/4]);
    }
      printf("%6d ", get_ku_pte_pfn(ku_h_memory+i));
    if(i%4==3)
     printf("\n");
  }
  printf("---------------------------------\n");
}

void show_swap(){
  for(int i=1;i<ku_h_swap_count;i++){
    printf("SFN %d : %d\n", i, ku_h_swapspace[i]);
  }
}
