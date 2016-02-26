#ifndef __NEXMON_FILTER_H__
#define __NEXMON_FILTER_H__

#define EINVAL 22
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))

struct sock_filter {    /* Filter block */
    uint16_t   code;   /* Actual filter code */
    uint8_t    jt; /* Jump true */
    uint8_t    jf; /* Jump false */
    uint32_t   k;      /* Generic multiuse field */
};

struct sock_fprog { /* Required for SO_ATTACH_FILTER. */
    unsigned short     len;    /* Number of filter blocks */
    struct sock_filter *filter;
};

/*
 * Instruction classes
 */

#define BPF_CLASS(code) ((code) & 0x07)
#define         BPF_LD          0x00
#define         BPF_LDX         0x01
#define         BPF_ST          0x02
#define         BPF_STX         0x03
#define         BPF_ALU         0x04
#define         BPF_JMP         0x05
#define         BPF_RET         0x06
#define         BPF_MISC        0x07

/* ld/ldx fields */
#define BPF_SIZE(code)  ((code) & 0x18)
#define         BPF_W           0x00
#define         BPF_H           0x08
#define         BPF_B           0x10
#define BPF_MODE(code)  ((code) & 0xe0)
#define         BPF_IMM         0x00
#define         BPF_ABS         0x20
#define         BPF_IND         0x40
#define         BPF_MEM         0x60
#define         BPF_LEN         0x80
#define         BPF_MSH         0xa0

/* alu/jmp fields */
#define BPF_OP(code)    ((code) & 0xf0)
#define         BPF_ADD         0x00
#define         BPF_SUB         0x10
#define         BPF_MUL         0x20
#define         BPF_DIV         0x30
#define         BPF_OR          0x40
#define         BPF_AND         0x50
#define         BPF_LSH         0x60
#define         BPF_RSH         0x70
#define         BPF_NEG         0x80
#define         BPF_JA          0x00
#define         BPF_JEQ         0x10
#define         BPF_JGT         0x20
#define         BPF_JGE         0x30
#define         BPF_JSET        0x40
#define BPF_SRC(code)   ((code) & 0x08)
#define         BPF_K           0x00
#define         BPF_X           0x08

/* ret - BPF_K and BPF_X also apply */
#define BPF_RVAL(code)  ((code) & 0x18)
#define         BPF_A           0x10

/* misc */
#define BPF_MISCOP(code) ((code) & 0xf8)
#define         BPF_TAX         0x00
#define         BPF_TXA         0x80

#define BPF_MAXINSNS 4096

/*
 * Macros for filter block array initializers.
 */
#define BPF_STMT(code, k) { (unsigned short)(code), 0, 0, k }
#define BPF_JUMP(code, k, jt, jf) { (unsigned short)(code), jt, jf, k }

/*
 * Number of scratch memory words for: BPF_ST and BPF_STX
 */
#define BPF_MEMWORDS 16

enum {
    BPF_S_RET_K = 1,
    BPF_S_RET_A,
    BPF_S_ALU_ADD_K,
    BPF_S_ALU_ADD_X,
    BPF_S_ALU_SUB_K,
    BPF_S_ALU_SUB_X,
    BPF_S_ALU_MUL_K,
    BPF_S_ALU_MUL_X,
    BPF_S_ALU_DIV_X,
    BPF_S_ALU_AND_K,
    BPF_S_ALU_AND_X,
    BPF_S_ALU_OR_K,
    BPF_S_ALU_OR_X,
    BPF_S_ALU_LSH_K,
    BPF_S_ALU_LSH_X,
    BPF_S_ALU_RSH_K,
    BPF_S_ALU_RSH_X,
    BPF_S_ALU_NEG,
    BPF_S_LD_W_ABS,
    BPF_S_LD_H_ABS,
    BPF_S_LD_B_ABS,
    BPF_S_LD_W_LEN,
    BPF_S_LD_W_IND,
    BPF_S_LD_H_IND,
    BPF_S_LD_B_IND,
    BPF_S_LD_IMM,
    BPF_S_LDX_W_LEN,
    BPF_S_LDX_B_MSH,
    BPF_S_LDX_IMM,
    BPF_S_MISC_TAX,
    BPF_S_MISC_TXA,
    BPF_S_ALU_DIV_K,
    BPF_S_LD_MEM,
    BPF_S_LDX_MEM,
    BPF_S_ST,
    BPF_S_STX,
    BPF_S_JMP_JA,
    BPF_S_JMP_JEQ_K,
    BPF_S_JMP_JEQ_X,
    BPF_S_JMP_JGE_K,
    BPF_S_JMP_JGE_X,
    BPF_S_JMP_JGT_K,
    BPF_S_JMP_JGT_X,
    BPF_S_JMP_JSET_K,
    BPF_S_JMP_JSET_X,
    /* Ancillary data */
    BPF_S_ANC_PROTOCOL,
    BPF_S_ANC_PKTTYPE,
    BPF_S_ANC_IFINDEX,
    BPF_S_ANC_NLATTR,
    BPF_S_ANC_NLATTR_NEST,
    BPF_S_ANC_MARK,
    BPF_S_ANC_QUEUE,
    BPF_S_ANC_HATYPE,
    BPF_S_ANC_RXHASH,
    BPF_S_ANC_CPU,
};

#endif /* __NEXMON_FILTER_H__ */
