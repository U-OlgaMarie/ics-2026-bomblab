#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    (void)challenge;
    // TODO: 根据 phase_01 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t c0  = challenge & 0xffffffff;
    uint32_t c1  = challenge >> 32;
    uint32_t c16 = (challenge >> 16) & 0xffffffff;

    uint32_t step0 = (c0 * 5 + c1) ^ 0x3848u;
    uint32_t step1 = (c16 + 0x30d0u) ^ c1;
    uint32_t step2 = (step1 * 5) ^ (c0 + 0xef89u);
    values[0] = step0 ^ step2;

    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 6;
    (void)challenge;
    // TODO: 根据 phase_02 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t v[6];
    v[0] = (uint32_t)((challenge >> 6) & 0x1f) + 9;
    values[0]=v[0];
    for (int i = 1; i < 6; ++i) {
    uint32_t window = (uint32_t)((challenge >> (9 + 5 * (i - 1))) & 0x1f);
    v[i] = 5 * v[i - 1] + (window ^ (uint32_t)(i + 4));
    values[i]=v[i];
    }
    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;
    (void)challenge;
    // TODO: 根据 phase_03 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t sw = (uint32_t)challenge & 0x7u;
    uint32_t x  = (uint32_t)(challenge >> 3) ^ (uint32_t)(challenge >> 32);

    uint32_t r;
    switch (sw) {
    case 0:  r = (x ^ 0x96dfu) + (x >> 1);  break;   /* p3_case_7 */
    case 1:  r = (x ^ 0x7ec3u) + (x >> 2);  break;   /* p3_case_3 */
    case 2:  r = (4u * x) ^ 0x33cfu;        break;   /* p3_case_6 */
    case 3:  r = 5u * x + 0x9501u;          break;   /* p3_case_5 */
    case 4:  r = 3u * x + 0x179eu;          break;   /* p3_case_1 */
    case 5:  r = 5u * x + 0x3d0e5u;         break;   /* p3_case_0 */
    case 6:  r = (2u * x) ^ 0xf387u;        break;   /* p3_case_2 */
    default: r = 3u * x + 0xd8ddu;          break;   /* p3_case_4，sw == 7 */
    }
    values[0] = sw;
    values[1] = r;
    return write_answer(out, cap, values, count);
}

static uint32_t p4_walk(uint32_t a, uint32_t b, uint32_t c, uint32_t key) {
    uint32_t mid = b + ((c - b) >> 1);
    if (mid == a) return (mid + 0x73u) ^ key;/* 402390 */
    uint32_t cterm = mid + key;
    if (a < mid)
        return (5u * p4_walk(a, b, mid - 1, key)) ^ cterm ^ 1u;/* 402370 分支 */
    return (5u * p4_walk(a, mid + 1, c, key)) ^ cterm ^ 2u;/* 402352 分支 */
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    (void)challenge;
    // TODO: 根据 phase_04 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t a   = (uint32_t)challenge & 0x7fu;/* 4023fb */
    uint32_t key = (uint32_t)(challenge >> 7) ^ (uint32_t)(challenge >> 32);/* 4023f7/4023f3/402403 */
    values[0] = p4_walk(a, 0u, 0x7fu, key); 
    return write_answer(out, cap, values, count);
}

static const struct { uint32_t b, c; } p5_nodes[12] = {
    {2600u, 42397u}, {2950u, 33530u}, {2330u, 58296u}, {  60u, 25581u},
    {1611u, 58210u}, { 533u, 20617u}, {1052u, 64559u}, {2108u, 14904u},
    { 847u,  3050u}, {1823u, 42938u}, {1316u, 64417u}, { 319u, 52207u},
};

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;
    (void)challenge;
    // TODO: 根据 phase_05 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    size_t   i    = (size_t)((uint32_t)challenge & 7u);
    uint32_t hops = ((uint32_t)challenge >> 3) & 3u;/* 402430/40245b：额外节点数 */
    uint32_t acc  = (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32);/* 40243c-402447 */

    acc ^= 0x91u;
    acc = (acc * 3u + p5_nodes[i].c) ^ p5_nodes[i].b;
    values[1] = p5_nodes[i].b;

    for (uint32_t k = 1; k <= hops; ++k) {/* 402460-40247b */
        ++i;
        acc = (acc * 3u + p5_nodes[i].c) ^ p5_nodes[i].b;
        values[1] = p5_nodes[i].b;
    }
    values[0] = acc;
    return write_answer(out, cap, values, count);
}

static const uint8_t p6_map[8] = {2u, 5u, 0u, 1u, 3u, 4u, 0u, 0u};

static uint32_t p6_eval(uint32_t op, uint32_t x) {
    switch (op) {
    case 0:  return ((x << 7) | (x >> 25)) ^ 0x3e28u;   /* p6_op_0: rol 7 */
    case 1:  return ((x >> 7) ^ x) + 0xfc35u;           /* p6_op_1 */
    case 2:  return (x >> 1) | (x << 31);               /* p6_op_2: ror 1 */
    case 3:  return (x + 0x7a4eu) ^ (x >> 3);           /* p6_op_3 */
    case 4:  return (x ^ 0x5b9eu) + (x << 5);           /* p6_op_4 */
    default: return ((x << 5) + 0xa52bu) ^ (7u * x);    /* p6_op_5 */
    }
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    (void)challenge;
    // TODO: 根据 phase_06 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t x = (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32);  /* 40258c-402597 */
    uint32_t r = p6_eval(p6_map[(uint32_t)challenge & 7u], x);              /* 4025a3/4025a6 */
    r = p6_eval(p6_map[((uint32_t)challenge >> 3) & 7u], r ^ 0xe892u);      /* 4025ad-4025b4 */
    values[0] = r + 0xcb90u;  
    return write_answer(out, cap, values, count);
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    (void)challenge;
    (void)context;
    (void)out;
    (void)cap;
    // TODO（选做）: 根据 secret_gate、secret_copy 和 secret_phase 的汇编构造完整的第七行，写入 out 后返回 0。
    // 第七行不一定只有整数，可以直接用 snprintf 写入；还没完成时保持返回 1，测试会显示 not attempted。
    uint32_t c0    = (uint32_t)context & 3u;                            /* 4026a5 */
    uint32_t ha    = (uint32_t)challenge ^ (uint32_t)(challenge >> 32); /* 4026ba-4026c2 */
    uint32_t hb    = (uint32_t)context   ^ (uint32_t)(context >> 32);   /* 4026be-4026c5 */
    uint32_t win16 = (uint32_t)(context >> 16);                         /* 4026c8-4026ce */

    uint32_t t   = ha ^ hb;
    uint32_t rot = (t >> 1) | (t << 31);                                /* 4026d4 secret_mix */
    uint32_t c1  = (rot ^ 0x886eu) ^ (0xf684u * c0) ^ win16;            /* 4026b0 / 4026d9 */
    uint32_t c2  = c1 ^ ((0x7eeau + hb + c0) + (ha >> 1));              /* 4026e1-4026ed */

    if (cap <= 32) return 1;
    memset(out, '.', 32);
    int n = snprintf(out + 32, cap - 32, "OPEN%" PRIu32 " %" PRIu32 " %" PRIu32, c0, c1, c2);
    return (n < 0 || (size_t)n >= cap - 32) ? 1 : 0;
    //return 1;
}
