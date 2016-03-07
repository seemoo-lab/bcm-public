#include "../include/bcm4339.h" /* needs to be imported before wrapper.h to define addresses */
#include "../include/wrapper.h"
#include "../include/helper.h"
#include "../include/structs.h"
#include "../include/nexmon_filter.h"

// test if the wlc_pub struct got the correct size, 
// WARNING: compilation fails if not
typedef char assertion_on_mystruct[ (sizeof(struct wlc_pub)==232)*2-1 ];


//from skbuff.h
inline void *
skb_header_pointer(const struct sk_buff *skb, int offset, int len, void *buffer) {
    return skb->data + offset;
}

inline void *
load_pointer(const struct sk_buff *skb, int k, unsigned int size, void *buffer) {
    if (k >= 0) {
        return skb_header_pointer(skb, k, size, buffer);
    }
    return 0;
    // k should be always be >= 0 in our case, we should be able to skip this for now
    //return bpf_internal_load_pointer_neg_helper(skb, k, size);
}

// Corresponds to net/core/filter.c:sk_chk_filter()
// to substitue the instructions with the enum BPF_S...
// decreases the amount of generated code!!!
inline int 
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


// return == 0 => throw packet away; return != 0 => keep it
inline unsigned int 
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
        printf("current code: %u, A: 0x%x, X: 0x%x, k: 0x%x\n", fentry->code, A, X, K);

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
        /* some thing is wrong here, see: https://stackoverflow.com/questions/6576517/what-is-the-cause-of-not-being-able-to-divide-numbers-in-gcc
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
        */
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
            //printf("BPF_S_JMP_JEQ_K: A == K? A: 0x%x, K: 0x%x\n", A, K);
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
                A = *((uint32_t *) ptr);
                continue;
            }
            return 0;
        case BPF_S_LD_H_ABS:
            k = K;
load_h:
            ptr = load_pointer(skb, k, 2, &tmp);
            if (ptr != 0) {
                //A = get_unaligned16(ptr);
                A = *((uint16_t *) ptr);
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
            printf("sk_run_filter(): Unknown code:0x%x jt:0x%x tf:0x%x k:0x%x\n",
                       fentry->code, fentry->jt,
                       fentry->jf, fentry->k);
            return 0;
        }
    }

    return 0;
}

unsigned int
nexmon_filter(struct sk_buff *skb, struct sock_filter *filter) {
    //int strip_hdr = 38;
    int strip_hdr = 42;
    char old_val0 = 0x0;
    char old_val1 = 0x0;
    char old_val2 = 0x0;
    char old_val3 = 0x0;
    void *orig_data;

    // equals: tcpdump -i mon0 -d wlan addr1 ff:ff:ff:ff:ff:ff
    struct sock_filter code[] = {
        { 0x30, 0, 0, 0x00000003 }, 
        { 0x64, 0, 0, 0x00000008 }, 
        { 0x7, 0, 0, 0x00000000 },
        { 0x30, 0, 0, 0x00000002 }, 
        { 0x4c, 0, 0, 0x00000000 },
        { 0x2, 0, 0, 0x00000000 },  
        { 0x7, 0, 0, 0x00000000 },  
        { 0x40, 0, 0, 0x00000006 }, 
//      MAC ADDRESS[3-6] HERE 
        { 0x15, 0, 3, 0xffffffff },
        { 0x48, 0, 0, 0x00000004 }, 
//      MAC ADDRESS[1-2] HERE 
        { 0x15, 0, 1, 0x0000ffff },
        { 0x6, 0, 0, 0x00040000 },
        { 0x6, 0, 0, 0x00000000 },
    };

    filter = code;

    short rxstat = *((short *)(skb->data + 0x10));
    if(rxstat & 4) {
        //printf("add two!\n");
        strip_hdr += 2;
    }

    // the filter needs to know where the the data starts in skb->data (presumably from a radiotap header)
    // 'strip_hdr' already jumps over the headers, hence we simly add
    // 4 byte to header that tell him to jump over this
    old_val0 = *((char *)(skb->data + strip_hdr + 0));
    *((char *)(skb->data + strip_hdr + 0)) = 0x0;
    old_val1 = *((char *)(skb->data + strip_hdr + 1));
    *((char *)(skb->data + strip_hdr + 1)) = 0x0;
    old_val2 = *((char *)(skb->data + strip_hdr + 2));
    *((char *)(skb->data + strip_hdr + 2)) = 0x4;
    old_val3 = *((char *)(skb->data + strip_hdr + 3));
    *((char *)(skb->data + strip_hdr + 3)) = 0x0;

    orig_data = skb->data;
    skb->data = skb->data + strip_hdr;
    //hexdump(0, skb->data, 0x40);

    //TODO get length right
    int ret = import_filter(filter, ARRAY_SIZE(code));

    if(ret == 0) {
        ret = sk_run_filter(skb, filter);
        if(ret != 0) {
            *((char *)(skb->data + 0)) = old_val0;
            *((char *)(skb->data + 1)) = old_val1;
            *((char *)(skb->data + 2)) = old_val2;
            *((char *)(skb->data + 3)) = old_val3;

            skb->data = orig_data;
        }
        return ret;
    } else {
        printf("ERROR Could not import filter: %d!\n", ret);
        return 0;
    }
}

int wlc_bmac_recv(struct wlc_hw_info *wlc_hw, unsigned int fifo, int bound, int *processed_frame_cnt) {

    struct wlc_pub *pub = wlc_hw->wlc->pub;
    sk_buff *p;
    char is_amsdu = pub->is_amsdu;
    int n = 0;
    int bound_limit;
    if(bound) {
        bound_limit = pub->tunables->rxbnd;
    } else {
        bound_limit = -1;
    }
    do {
        p = dma_rx (wlc_hw->di[fifo]);
        if(!p) {
            goto LEAVE;
        }
        // nexmon filtering
        // 2nd parameter is zero => the filter is currently static
#ifdef NEXMON_FILTER
        if( nexmon_filter(p, 0) == 0 ) {
            printf("FILTER: tossed!\n");
            ++n;
            pkt_buf_free_skb(wlc_hw->wlc->osh, p, 0);
            goto LEAVE;
        } else {
            printf("FILTER: keep!\n");
        }
#endif /* NEXMON_FILTER */

        if(is_amsdu) {
            is_amsdu = 0;
        }
        dngl_sendpkt(SDIO_INFO_ADDR, p, 0xF);
        ++n;
    } while(n < bound_limit);
LEAVE:
    dma_rxfill(wlc_hw->di[fifo]);
    wlc_bmac_mctrl(wlc_hw, 0x41d60000, 0x41d20000);
    *processed_frame_cnt += n;
    if ( n < bound_limit ) {
        return 0;
    } else {
        return 1;
    }
}

