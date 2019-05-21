int ku_traverse(void *ku_cr3, char va, void *pmem) {
    void *pte;
    char data;
    char pfn;
    
    char offset_pd = (va & 0xC0) >> 6;   // page directory offset 11000000
    char offset_pmd = (va & 0x30) >> 4;  // page middle directory offset 00110000
    char offset_pt = (va & 0x0C) >> 2;   // page table offset == VPN 00001100
    char offset = (va & 0x03);           // 00000011

    if(pmem == NULL)
        return 0;
    
    pte = ku_cr3 + offset_pd; // page directory의 pde 탐색
    data = *((char*)pte);
    if((data & 1) == 0)
        return 0;
    else if((data & 0x2) != 0)
        return 0;
    else {
        pfn = (data & 0xfc) >> 2; // pfn
        pte = pmem + (pfn * 4) + offset_pmd; // page middle directory의 pmde 탐색
        data = *((char*)pte);
        if((data & 1) == 0)
            return 0;
        else if((data & 0x2) != 0)
            return 0;
        else {
            pfn = (data & 0xfc) >> 2;
            pte = pmem + (pfn * 4) + offset_pt; // page table의 pte 탐색
            data = *((char*)pte); // 매핑된 페이지
            if((data & 1) == 0)
                return 0;
            else if((data & 0x2) != 0) // present가 1이면서 0x2마스킹한 비트가 0이면 xxxxxx11이니까 false
                return 0;
            else {
                return (data & 0xfc) + offset; // 결과값은 pte에 저장된 PFN * 4 + offset(즉 실제 메모리의 PFN과 offset)
            }
        }
    }
    return 0;
}
