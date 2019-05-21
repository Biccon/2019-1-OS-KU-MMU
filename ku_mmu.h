struct ku_pte {
  unsigned char data;
};

typedef struct ku_h_pcb
{
    char pid;             // process pid
    int pdba; // page directory base address -> offset
} ku_h_pcb;

typedef struct ku_h_node
{
    ku_h_pcb *pcb;
    struct ku_h_node *prev;
    struct ku_h_node *next;
} ku_h_node;

typedef struct ku_h_linkedlist
{
    ku_h_node *header;
    ku_h_node *tailer;
} ku_h_linkedlist;

/* Global Variables */
struct ku_pte* ku_h_memory;
int* ku_h_memory_swapable;
int* ku_h_swapspace;
ku_h_linkedlist* ku_h_processes;

unsigned int ku_h_mem_size;  // 40 byte
unsigned int ku_h_swap_size;
unsigned int ku_h_page_size = 4;
unsigned int ku_h_page_count;
unsigned int ku_h_swap_count;
unsigned int ku_h_page_index = 1;
unsigned int ku_h_swap_index = 1;

/* LinkedList Functions */
ku_h_pcb *ku_h_make_pcb(char pid)
{
    ku_h_pcb *new_pcb = (ku_h_pcb *)malloc(sizeof(ku_h_pcb));
    new_pcb->pid = pid;
    new_pcb->pdba = 0;
    return new_pcb;
}

ku_h_node *ku_h_add_before(ku_h_node *node, char pid)
{
    ku_h_node *new_node = (ku_h_node *)malloc(sizeof(ku_h_node));
    new_node->pcb = ku_h_make_pcb(pid);
    new_node->prev = node->prev;
    new_node->next = node;

    node->prev->next = new_node;
    node->prev = new_node;
    return new_node;
}

ku_h_node *ku_h_add_last(ku_h_linkedlist *list, char pid)
{
    return ku_h_add_before(list->tailer, pid);
}

ku_h_node *ku_h_get_node_by_index(ku_h_linkedlist *list, int index)
{
    ku_h_node *temp = list->header->next;
    for (int i = 0; (i < index && temp != list->tailer); i++)
    {
        temp = temp->next;
    }
    return temp;
}

ku_h_node *ku_h_get_node_by_pid(ku_h_linkedlist *list, char pid)
{
    ku_h_node *temp = list->header->next;
    while (temp != list->tailer)
    {
        if (temp->pcb->pid == pid)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

void ku_h_init_list(ku_h_linkedlist *list)
{
    list->header = (ku_h_node *)malloc(sizeof(ku_h_node));
    list->tailer = (ku_h_node *)malloc(sizeof(ku_h_node));

    list->header->prev = NULL;
    list->header->next = list->tailer;

    list->tailer->prev = list->header;
    list->tailer->next = NULL;
}
/* PTE Functions */
void set_ku_pte(struct ku_pte* pte, char a, char b) {
  pte->data = a | b;
}

void set_ku_pte_pfn(struct ku_pte* pte, char pfn) {
  set_ku_pte(pte, pfn << 2, 1);
}

void set_ku_pte_swap_offset(struct ku_pte* pte, char swap_offset) {
  set_ku_pte(pte, swap_offset << 1, 0);
}

unsigned char get_ku_pte_pfn(struct ku_pte* pte) {
  return (pte->data) >> 2;
}

unsigned char get_ku_pte_swap_offset(struct ku_pte* pte) {
  return (pte->data) >> 1;
}

unsigned char get_ku_pte_present(struct ku_pte* pte) { return pte->data & 0x1; }


/* Getting page Functions */
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
  if (get_count(0) == 0) 
  {
    if (get_count(-1) != ku_h_page_count) 
    {
      pfn = get_firstin_page();
      ku_h_memory_swapable[pfn] =
          (swappable == -1) ? swappable
                            : ku_h_page_index++;
      int sfn = 0;
      for(int i=1;i<ku_h_swap_count;i++){ 
        if(ku_h_swapspace[i] == 0){
          sfn = i;
          ku_h_swapspace[i] = ku_h_swap_index++; 
          break;
        }
      }
      int temp_index_of_pt = ku_h_memory[pfn*4].data;
      set_ku_pte_swap_offset(ku_h_memory+temp_index_of_pt, sfn);
      ku_h_memory[pfn*4].data = 0;
      ku_h_memory[pfn*4+1].data = 0;
      ku_h_memory[pfn*4+2].data = 0;
      ku_h_memory[pfn*4+3].data = 0;
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

  return pfn;
}

int ku_page_fault(char pid, char va) {
  ku_h_node* process = ku_h_get_node_by_pid(ku_h_processes, pid);
  ku_h_pcb* pcb = process->pcb;

  char offset_pd = (va & 0xC0) >> 6;   // page directory offset 11000000
  char offset_pmd = (va & 0x30) >> 4;  // page middle directory offset 00110000
  char offset_pt = (va & 0x0C) >> 2;   // page table offset == VPN 00001100
  char offset = (va & 0x03);           // 00000011
  struct ku_pte* pd =ku_h_memory + pcb->pdba + offset_pd;  

  int pfn_of_pmd = -1; 
  if (pd->data == 0) {  
    pfn_of_pmd = get_page(-1); 
    if (pfn_of_pmd == -1) {
      return -1;
    } else {
      set_ku_pte_pfn(
          pd,
          pfn_of_pmd); 
       
      ku_h_memory[pfn_of_pmd*4].data = 0; 
      ku_h_memory[pfn_of_pmd*4+1].data = 0;
      ku_h_memory[pfn_of_pmd*4+2].data = 0;
      ku_h_memory[pfn_of_pmd*4+3].data = 0;
    }
  } else {
    char present_of_pd = get_ku_pte_present(pd);
    if (present_of_pd == 0) {
      // page directory는 스왑되지 않으므로 공백
    } else {
      pfn_of_pmd = get_ku_pte_pfn(pd);
    }
  }

  struct ku_pte* pmd = (ku_h_memory + pfn_of_pmd*4) + offset_pmd;
  int pfn_of_pt = -1;
  if (pmd->data == 0) {
    pfn_of_pt = get_page(-1);
    if (pfn_of_pt == -1) {
      return -1;
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
      // pmd 스왑 안되므로 공백
    } else {
      pfn_of_pt = get_ku_pte_pfn(pmd);
    }
  }

  struct ku_pte* pt = (ku_h_memory + pfn_of_pt*4) + offset_pt;
  int pfn_of_page = -1;
  if (pt->data == 0) { 
    pfn_of_page = get_page(0);
    if (pfn_of_page == -1) {
      return -1; 
    } else {
      set_ku_pte_pfn(pt, pfn_of_page);
      ku_h_memory[pfn_of_page*4].data = pfn_of_pt*4+offset_pt;
      ku_h_memory[pfn_of_page*4+1].data = 0;
      ku_h_memory[pfn_of_page*4+2].data = 0;
      ku_h_memory[pfn_of_page*4+3].data = 0;
    }
  } else {
    char present_of_page = get_ku_pte_present(pt); 
    if (present_of_page == 0) {
      pfn_of_page = get_page(0);
      if (pfn_of_page == -1) {
        return -1;
      } else {
        ku_h_swapspace[get_ku_pte_swap_offset(pt)] = 0;
        set_ku_pte_pfn(
            pt,
            pfn_of_page);  
        ku_h_memory[pfn_of_page*4].data = pfn_of_pt*4+offset_pt;
      }
    } else {
      // 매핑된 페이지에 내용은 중요하지 않은 값.
    }
  }
  return 0;
}

void* ku_mmu_init(unsigned int mem_size, unsigned int swap_size) {
  ku_h_mem_size = mem_size;
  ku_h_page_count = mem_size / ku_h_page_size;
  ku_h_swap_size = swap_size;
  ku_h_swap_count = swap_size / ku_h_page_size;

  ku_h_processes = (ku_h_linkedlist*)malloc(sizeof(ku_h_linkedlist));
  ku_h_memory = (struct ku_pte*)calloc(mem_size, sizeof(struct ku_pte)); 
  ku_h_memory_swapable =
      (int*)calloc(ku_h_page_count, sizeof(int)); 
  ku_h_swapspace =
      (int*)calloc(ku_h_swap_count, sizeof(int)); 

  ku_h_init_list(ku_h_processes);

  return (void*)ku_h_memory;
}

int ku_run_proc(char pid, struct ku_pte** ku_cr3) {
  ku_h_node* process = ku_h_get_node_by_pid(ku_h_processes, pid);
  if (process == NULL) {
    int pfn_for_pd = get_page(
        -1); 
    if (pfn_for_pd == -1) {
      return -1;
    } else {
      process = ku_h_add_last(ku_h_processes, pid);
      process->pcb = ku_h_make_pcb(pid);
      process->pcb->pdba = pfn_for_pd * 4;
    }
  }
  *ku_cr3 = (struct ku_pte*)(ku_h_memory+process->pcb->pdba); 
  return 0;
}