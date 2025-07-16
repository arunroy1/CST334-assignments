#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#include "student_code.h"

bool is_entry_valid(PageTableEntry pte) {
    return is_bit_set(pte, VALID_BIT);
}

bool is_read_enabled(PageTableEntry pte) {
    return is_bit_set(pte, READ_BIT);
}

bool is_write_enabled(PageTableEntry pte) {
    return is_bit_set(pte, WRITE_BIT);
}

bool is_execute_enabled(PageTableEntry pte) {
    return is_bit_set(pte, EXEC_BIT);
}

VPN get_vpn_from_va(VirtualAddress va) {
    return (VPN)(va >> NUM_OFFSET_BITS);
}

PFN find_free_page(MMU m) {
    for (size_t i = 0; i < NUM_FRAMES; i++) {
        if (!m.page_used[i]) {
            return (PFN)i;
        }
    }
    return (PFN)0; // no free page found
}

PFN convert_PageTableEntry_to_PFN(PageTableEntry pte) {
    return (PFN)(pte & get_mask(NUM_PFN_BITS + NUM_OFFSET_BITS));
}

PageTableEntry get_pagetableentry__MMU_pagetable(MMU m, VPN vpn) {
    return m.page_pointer[vpn];
}

PFN map_page__MMU_pagetable(MMU* m, VPN vpn, bool can_read, bool can_write, bool can_exec) {
    PFN pfn = find_free_page(*m);
    m->page_used[pfn] = true;
    PageTableEntry pte = (PageTableEntry)pfn;
    // set metadata bits
    pte |= get_mask_single_bit(VALID_BIT);
    if (can_read)  pte |= get_mask_single_bit(READ_BIT);
    if (can_write) pte |= get_mask_single_bit(WRITE_BIT);
    if (can_exec)  pte |= get_mask_single_bit(EXEC_BIT);
    m->page_pointer[vpn] = pte;
    return pfn;
}

Page* get_page(MMU m, VirtualAddress va, bool for_read, bool for_write, bool for_execute) {
    VPN vpn = get_vpn_from_va(va);
    PageTableEntry pte = m.get_pagetableentry(m, vpn);
    // permission checks
    if (!is_entry_valid(pte))      return NULL;
    if (for_read && !is_read_enabled(pte))      return NULL;
    if (for_write && !is_write_enabled(pte))    return NULL;
    if (for_execute && !is_execute_enabled(pte)) return NULL;
    PFN pfn = convert_PageTableEntry_to_PFN(pte);
    char* base = m.physical_memory + ((size_t)pfn << NUM_OFFSET_BITS);
    return (Page*)base;
}

char read_byte(MMU m, VirtualAddress va) {
    Page* page = get_page(m, va, true, false, false);
    OFFSET off = (OFFSET)(va & get_mask(NUM_OFFSET_BITS));
    return page->data[off];
}

void write_byte(MMU m, VirtualAddress va, char val) {
    Page* page = get_page(m, va, false, true, false);
    OFFSET off = (OFFSET)(va & get_mask(NUM_OFFSET_BITS));
    page->data[off] = val;
}
