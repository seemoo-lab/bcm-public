#include "../include/bcm4339.h"
#include "../include/wrapper.h"
#include "../include/structs.h"
#include "../include/helper.h"
#include "../include/types.h" /* needs to be included before bcmsdpcm.h */
#include "nexmon_filter.h"

/**
 * Corresponds to net/core/filter.c:sk_chk_filter()
 */
int 
import_filter(struct sock_filter *filter, unsigned int flen) {
    /*
     * Valid instructions are initialized to non-0.
     * Invalid instructions are initialized to 0.
     */
    static const uint8_t codes[] = {
        [BPF_ALU|BPF_ADD|BPF_K]  = BPF_S_ALU_ADD_K,
        [BPF_ALU|BPF_ADD|BPF_X]  = BPF_S_ALU_ADD_X,
        [BPF_ALU|BPF_SUB|BPF_K]  = BPF_S_ALU_SUB_K,
        [BPF_ALU|BPF_SUB|BPF_X]  = BPF_S_ALU_SUB_X,
        [BPF_ALU|BPF_MUL|BPF_K]  = BPF_S_ALU_MUL_K,
        [BPF_ALU|BPF_MUL|BPF_X]  = BPF_S_ALU_MUL_X,
        [BPF_ALU|BPF_DIV|BPF_X]  = BPF_S_ALU_DIV_X,
        [BPF_ALU|BPF_AND|BPF_K]  = BPF_S_ALU_AND_K,
        [BPF_ALU|BPF_AND|BPF_X]  = BPF_S_ALU_AND_X,
        [BPF_ALU|BPF_OR|BPF_K]   = BPF_S_ALU_OR_K,
        [BPF_ALU|BPF_OR|BPF_X]   = BPF_S_ALU_OR_X,
        [BPF_ALU|BPF_LSH|BPF_K]  = BPF_S_ALU_LSH_K,
        [BPF_ALU|BPF_LSH|BPF_X]  = BPF_S_ALU_LSH_X,
        [BPF_ALU|BPF_RSH|BPF_K]  = BPF_S_ALU_RSH_K,
        [BPF_ALU|BPF_RSH|BPF_X]  = BPF_S_ALU_RSH_X,
        [BPF_ALU|BPF_NEG]        = BPF_S_ALU_NEG,
        [BPF_LD|BPF_W|BPF_ABS]   = BPF_S_LD_W_ABS,
        [BPF_LD|BPF_H|BPF_ABS]   = BPF_S_LD_H_ABS,
        [BPF_LD|BPF_B|BPF_ABS]   = BPF_S_LD_B_ABS,
        [BPF_LD|BPF_W|BPF_LEN]   = BPF_S_LD_W_LEN,
        [BPF_LD|BPF_W|BPF_IND]   = BPF_S_LD_W_IND,
        [BPF_LD|BPF_H|BPF_IND]   = BPF_S_LD_H_IND,
        [BPF_LD|BPF_B|BPF_IND]   = BPF_S_LD_B_IND,
        [BPF_LD|BPF_IMM]         = BPF_S_LD_IMM,
        [BPF_LDX|BPF_W|BPF_LEN]  = BPF_S_LDX_W_LEN,
        [BPF_LDX|BPF_B|BPF_MSH]  = BPF_S_LDX_B_MSH,
        [BPF_LDX|BPF_IMM]        = BPF_S_LDX_IMM,
        [BPF_MISC|BPF_TAX]       = BPF_S_MISC_TAX,
        [BPF_MISC|BPF_TXA]       = BPF_S_MISC_TXA,
        [BPF_RET|BPF_K]          = BPF_S_RET_K,
        [BPF_RET|BPF_A]          = BPF_S_RET_A,
        [BPF_ALU|BPF_DIV|BPF_K]  = BPF_S_ALU_DIV_K,
        [BPF_LD|BPF_MEM]         = BPF_S_LD_MEM,
        [BPF_LDX|BPF_MEM]        = BPF_S_LDX_MEM,
        [BPF_ST]                 = BPF_S_ST,
        [BPF_STX]                = BPF_S_STX,
        [BPF_JMP|BPF_JA]         = BPF_S_JMP_JA,
        [BPF_JMP|BPF_JEQ|BPF_K]  = BPF_S_JMP_JEQ_K,
        [BPF_JMP|BPF_JEQ|BPF_X]  = BPF_S_JMP_JEQ_X,
        [BPF_JMP|BPF_JGE|BPF_K]  = BPF_S_JMP_JGE_K,
        [BPF_JMP|BPF_JGE|BPF_X]  = BPF_S_JMP_JGE_X,
        [BPF_JMP|BPF_JGT|BPF_K]  = BPF_S_JMP_JGT_K,
        [BPF_JMP|BPF_JGT|BPF_X]  = BPF_S_JMP_JGT_X,
        [BPF_JMP|BPF_JSET|BPF_K] = BPF_S_JMP_JSET_K,
        [BPF_JMP|BPF_JSET|BPF_X] = BPF_S_JMP_JSET_X,
    };
    int pc;

    if (flen == 0 || flen > BPF_MAXINSNS) {
        return -EINVAL;
    }

    /* check the filter code now */
    for (pc = 0; pc < flen; pc++) {
        struct sock_filter *ftest = &filter[pc];
        uint16_t code = ftest->code;

        if (code >= ARRAY_SIZE(codes)) {
            return -EINVAL;
        }
        code = codes[code];
        if (!code) {
            return -EINVAL;
        }
        /* special checks for some instructions would begin here, we dont do that */
        // changes input!
        ftest->code = code;
    }
    /* there would be some checks if the last instruction are return commands, we dont do that either */
    return 0;
}

void*
receive_filter(void *sdio, sk_buff *p) {
    int chan = *((int *)(p->data + 1)) & 0xf;
    //void *data_payload = (void *)(p->data + 8);
    int offset = 0;
    if(*((int *)(sdio + 0x220))) {
        offset = *((int *)(p->data + 3)) - 20;
    } else {
        offset = *((int *)(p->data + 3)) - 12;
    }

    //p->data = data_payload + offset;
    //p->len = p->len - 8 - offset;

    if(chan == 4) {
        printf("our filter!: %d\n", chan);
        hexdump(0, p->data + 8 + offset, p->len - 8 - offset);
        //hexdump(0, p->data, p->len);
    } else {
        printf("some other stuff... %d\n", chan);
    }
    
    return sdio_header_parsing_from_sk_buff(sdio, p);
}
