#include "../include/bcm4339.h"
#include "../include/wrapper.h"
#include "../include/structs.h"
#include "../include/helper.h"
#include "../include/types.h" /* needs to be included before bcmsdpcm.h */
#include "nexmon_filter.h"

static inline uint32_t 
reciprocal_divide(uint32_t A, uint32_t R) {
    return (uint32_t)(((uint64_t)A * R) >> 32);
}

//static inline uint16_t
//get_unaligned16(const void *p) {
//        const uint8_t *_p = p;
//        return _p[0] << 8 | _p[1];
//}

//static inline uint32_t
//get_unaligned32(const void *p) {
//    uint32_t val = (uint32_t) p;
//    __asm (" ldnw     .d1t1   *%0,%0\n"
//         " nop     4\n"
//         : "+a"(val));
//    return val;
//}

//from skbuff.h
//static inline unsigned char *
//skb_network_header(const struct sk_buff *skb) {
//    return skb->head + skb->network_header;
//}
//static inline unsigned char *
//skb_mac_header(const struct sk_buff *skb) {
//    return skb->head + skb->mac_header;
//}
//static inline unsigned char *
//skb_tail_pointer(const struct sk_buff *skb) {
//    return skb->head + skb->tail;
//}

//void *
//bpf_internal_load_pointer_neg_helper(const struct sk_buff *skb, int k, unsigned int size) {
//    uint8_t *ptr = 0;
//
//    if (k >= SKF_NET_OFF)
//        ptr = skb_network_header(skb) + k - SKF_NET_OFF;
//    else if (k >= SKF_LL_OFF)
//        ptr = skb_mac_header(skb) + k - SKF_LL_OFF;
//
//    if (ptr >= skb->head && ptr + size <= skb_tail_pointer(skb))
//        return ptr;
//    return 0;
//}

//from skbuff.h
static inline void *
skb_header_pointer(const struct sk_buff *skb, int offset, int len, void *buffer) {
    int hlen = skb->len - skb->data_len;

    if (hlen - offset >= len) {
        return skb->data + offset;
    } else {
        printf("skb_header_pointer(): ERROR, this is not covered yet!\n");
    }
    // maybe not needed
    //if (skb_copy_bits(skb, offset, buffer, len) < 0) {
    //    return 0;
    //}
    return buffer;
}

static inline void *
load_pointer(const struct sk_buff *skb, int k, unsigned int size, void *buffer) {
    if (k >= 0) {
        return skb_header_pointer(skb, k, size, buffer);
    }
    return 0;
    // k should be always be >= 0 in our case, we should be able to skip this for now
    //return bpf_internal_load_pointer_neg_helper(skb, k, size);
}

//Corresponds to net/core/filter.c:sk_chk_filter()
int 
import_filter(struct sock_filter *filter, unsigned int flen) {
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
        // special checks for some instructions would begin here, we dont do that
        // changes input!
        ftest->code = code;
    }
    // there would be some checks if the last instruction are return commands, we dont do that either
    return 0;
}

unsigned int 
sk_run_filter(const struct sk_buff *skb, const struct sock_filter *fentry) {
    void *ptr;
    uint32_t A = 0;          // Accumulator
    uint32_t X = 0;          // Index Register
    uint32_t mem[BPF_MEMWORDS];      // Scratch Memory Store
    uint32_t tmp;
    int k;

    // Process array of filter instructions.
    for (;; fentry++) {
        const uint32_t K = fentry->k;

        switch (fentry->code) {
        case BPF_S_ALU_ADD_X:
            A += X;
            continue;
        case BPF_S_ALU_ADD_K:
            A += K;
            continue;
        case BPF_S_ALU_SUB_X:
            A -= X;
            continue;
        case BPF_S_ALU_SUB_K:
            A -= K;
            continue;
        case BPF_S_ALU_MUL_X:
            A *= X;
            continue;
        case BPF_S_ALU_MUL_K:
            A *= K;
            continue;
        case BPF_S_ALU_DIV_X:
            if (X == 0)
                return 0;
            A /= X;
            continue;
        case BPF_S_ALU_DIV_K:
            A = reciprocal_divide(A, K);
            continue;
        case BPF_S_ALU_AND_X:
            A &= X;
            continue;
        case BPF_S_ALU_AND_K:
            A &= K;
            continue;
        case BPF_S_ALU_OR_X:
            A |= X;
            continue;
        case BPF_S_ALU_OR_K:
            A |= K;
            continue;
        case BPF_S_ALU_LSH_X:
            A <<= X;
            continue;
        case BPF_S_ALU_LSH_K:
            A <<= K;
            continue;
        case BPF_S_ALU_RSH_X:
            A >>= X;
            continue;
        case BPF_S_ALU_RSH_K:
            A >>= K;
            continue;
        case BPF_S_ALU_NEG:
            A = -A;
            continue;
        case BPF_S_JMP_JA:
            fentry += K;
            continue;
        case BPF_S_JMP_JGT_K:
            fentry += (A > K) ? fentry->jt : fentry->jf;
            continue;
        case BPF_S_JMP_JGE_K:
            fentry += (A >= K) ? fentry->jt : fentry->jf;
            continue;
        case BPF_S_JMP_JEQ_K:
            fentry += (A == K) ? fentry->jt : fentry->jf;
            continue;
        case BPF_S_JMP_JSET_K:
            fentry += (A & K) ? fentry->jt : fentry->jf;
            continue;
        case BPF_S_JMP_JGT_X:
            fentry += (A > X) ? fentry->jt : fentry->jf;
            continue;
        case BPF_S_JMP_JGE_X:
            fentry += (A >= X) ? fentry->jt : fentry->jf;
            continue;
        case BPF_S_JMP_JEQ_X:
            fentry += (A == X) ? fentry->jt : fentry->jf;
            continue;
        case BPF_S_JMP_JSET_X:
            fentry += (A & X) ? fentry->jt : fentry->jf;
            continue;
        case BPF_S_LD_W_ABS:
            k = K;
load_w:
            ptr = load_pointer(skb, k, 4, &tmp);
            if (ptr != 0) {
                //A = get_unaligned32(ptr);
                A = (uint32_t) ptr;
                continue;
            }
            return 0;
        case BPF_S_LD_H_ABS:
            k = K;
load_h:
            ptr = load_pointer(skb, k, 2, &tmp);
            if (ptr != 0) {
                //A = get_unaligned16(ptr);
                A = (uint32_t) ptr;
                continue;
            }
            return 0;
        case BPF_S_LD_B_ABS:
            k = K;
load_b:
            ptr = load_pointer(skb, k, 1, &tmp);
            if (ptr != 0) {
                A = *(uint8_t *)ptr;
                continue;
            }
            return 0;
        case BPF_S_LD_W_LEN:
            A = skb->len;
            continue;
        case BPF_S_LDX_W_LEN:
            X = skb->len;
            continue;
        case BPF_S_LD_W_IND:
            k = X + K;
            goto load_w;
        case BPF_S_LD_H_IND:
            k = X + K;
            goto load_h;
        case BPF_S_LD_B_IND:
            k = X + K;
            goto load_b;
        case BPF_S_LDX_B_MSH:
            ptr = load_pointer(skb, K, 1, &tmp);
            if (ptr != 0) {
                X = (*(uint8_t *)ptr & 0xf) << 2;
                continue;
            }
            return 0;
        case BPF_S_LD_IMM:
            A = K;
            continue;
        case BPF_S_LDX_IMM:
            X = K;
            continue;
        case BPF_S_LD_MEM:
            A = mem[K];
            continue;
        case BPF_S_LDX_MEM:
            X = mem[K];
            continue;
        case BPF_S_MISC_TAX:
            X = A;
            continue;
        case BPF_S_MISC_TXA:
            A = X;
            continue;
        case BPF_S_RET_K:
            return K;
        case BPF_S_RET_A:
            return A;
        case BPF_S_ST:
            mem[K] = A;
            continue;
        case BPF_S_STX:
            mem[K] = X;
            continue;
        //case BPF_S_ANC_PROTOCOL:
        //    A = ntohs(skb->protocol);
        //    continue;
        //case BPF_S_ANC_PKTTYPE:
        //    A = skb->pkt_type;
        //    continue;
        //case BPF_S_ANC_IFINDEX:
        //    if (!skb->dev)
        //        return 0;
        //    A = skb->dev->ifindex;
        //    continue;
        //case BPF_S_ANC_MARK:
        //    A = skb->mark;
        //    continue;
        //case BPF_S_ANC_QUEUE:
        //    A = skb->queue_mapping;
        //    continue;
        //case BPF_S_ANC_HATYPE:
        //    if (!skb->dev)
        //        return 0;
        //    A = skb->dev->type;
        //    continue;
        //case BPF_S_ANC_RXHASH:
        //    A = skb->rxhash;
        //    continue;
        //case BPF_S_ANC_CPU:
        //    A = raw_smp_processor_id();
        //    continue;
        //case BPF_S_ANC_NLATTR: {
        //    struct nlattr *nla;

        //    if (skb_is_nonlinear(skb))
        //        return 0;
        //    if (A > skb->len - sizeof(struct nlattr))
        //        return 0;

        //    nla = nla_find((struct nlattr *)&skb->data[A],
        //               skb->len - A, X);
        //    if (nla)
        //        A = (void *)nla - (void *)skb->data;
        //    else
        //        A = 0;
        //    continue;
        //}
        //case BPF_S_ANC_NLATTR_NEST: {
        //    struct nlattr *nla;

        //    if (skb_is_nonlinear(skb))
        //        return 0;
        //    if (A > skb->len - sizeof(struct nlattr))
        //        return 0;

        //    nla = (struct nlattr *)&skb->data[A];
        //    if (nla->nla_len > A - skb->len)
        //        return 0;

        //    nla = nla_find_nested(nla, X);
        //    if (nla)
        //        A = (void *)nla - (void *)skb->data;
        //    else
        //        A = 0;
        //    continue;
        //}
        default:
            printf("sk_run_filter(): Unknown code:%u jt:%u tf:%u k:%u\n",
                       fentry->code, fentry->jt,
                       fentry->jf, fentry->k);
            return 0;
        }
    }

    return 0;
}

void*
receive_filter(void *sdio, sk_buff *p) {
    //do the same as in the original function:
    int chan = *((int *)(p->data + 1)) & 0xf;
    int offset = 0;
    if(*((int *)(sdio + 0x220))) {
        offset = *((int *)(p->data + 3)) - 20;
    } else {
        offset = *((int *)(p->data + 3)) - 12;
    }

    if(chan == 4) {
        printf("our filter!: %d\n", chan);
        hexdump(0, p->data + 8 + offset, p->len - 8 - offset);
    } else {
        printf("even some other stuff... %d\n", chan);
    }
    
    return sdio_header_parsing_from_sk_buff(sdio, p);
}
