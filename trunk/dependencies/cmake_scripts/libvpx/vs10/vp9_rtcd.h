#ifndef VP9_RTCD_H_
#define VP9_RTCD_H_

#ifdef RTCD_C
#define RTCD_EXTERN
#else
#define RTCD_EXTERN extern
#endif

/*
 * VP9
 */

#include "vpx/vpx_integer.h"
#include "vp9/common/vp9_enums.h"

struct macroblockd;

/* Encoder forward decls */
struct macroblock;
struct vp9_variance_vtable;

#define DEC_MVCOSTS int *mvjcost, int *mvcost[2]
union int_mv;
struct yv12_buffer_config;

void vp9_idct_add_16x16_c(int16_t *input, uint8_t *dest, int stride, int eob);
#define vp9_idct_add_16x16 vp9_idct_add_16x16_c

void vp9_idct_add_8x8_c(int16_t *input, uint8_t *dest, int stride, int eob);
#define vp9_idct_add_8x8 vp9_idct_add_8x8_c

void vp9_idct_add_c(int16_t *input, uint8_t *dest, int stride, int eob);
#define vp9_idct_add vp9_idct_add_c

void vp9_idct_add_32x32_c(int16_t *q, uint8_t *dst, int stride, int eob);
#define vp9_idct_add_32x32 vp9_idct_add_32x32_c

void vp9_d27_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_d27_predictor_4x4 vp9_d27_predictor_4x4_c

void vp9_d45_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
void vp9_d45_predictor_4x4_ssse3(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
RTCD_EXTERN void (*vp9_d45_predictor_4x4)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

void vp9_d63_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_d63_predictor_4x4 vp9_d63_predictor_4x4_c

void vp9_h_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
void vp9_h_predictor_4x4_ssse3(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
RTCD_EXTERN void (*vp9_h_predictor_4x4)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

void vp9_d117_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_d117_predictor_4x4 vp9_d117_predictor_4x4_c

void vp9_d135_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_d135_predictor_4x4 vp9_d135_predictor_4x4_c

void vp9_d153_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_d153_predictor_4x4 vp9_d153_predictor_4x4_c

void vp9_v_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
void vp9_v_predictor_4x4_sse(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
RTCD_EXTERN void (*vp9_v_predictor_4x4)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

void vp9_tm_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
void vp9_tm_predictor_4x4_sse(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
RTCD_EXTERN void (*vp9_tm_predictor_4x4)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

void vp9_dc_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
void vp9_dc_predictor_4x4_sse(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
RTCD_EXTERN void (*vp9_dc_predictor_4x4)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

void vp9_dc_top_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_dc_top_predictor_4x4 vp9_dc_top_predictor_4x4_c

void vp9_dc_left_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_dc_left_predictor_4x4 vp9_dc_left_predictor_4x4_c

void vp9_dc_128_predictor_4x4_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_dc_128_predictor_4x4 vp9_dc_128_predictor_4x4_c

void vp9_d27_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_d27_predictor_8x8 vp9_d27_predictor_8x8_c

void vp9_d45_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
void vp9_d45_predictor_8x8_ssse3(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
RTCD_EXTERN void (*vp9_d45_predictor_8x8)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

void vp9_d63_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_d63_predictor_8x8 vp9_d63_predictor_8x8_c

void vp9_h_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
void vp9_h_predictor_8x8_ssse3(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
RTCD_EXTERN void (*vp9_h_predictor_8x8)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

void vp9_d117_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_d117_predictor_8x8 vp9_d117_predictor_8x8_c

void vp9_d135_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_d135_predictor_8x8 vp9_d135_predictor_8x8_c

void vp9_d153_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_d153_predictor_8x8 vp9_d153_predictor_8x8_c

void vp9_v_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
void vp9_v_predictor_8x8_sse(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
RTCD_EXTERN void (*vp9_v_predictor_8x8)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

void vp9_tm_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
void vp9_tm_predictor_8x8_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
RTCD_EXTERN void (*vp9_tm_predictor_8x8)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

void vp9_dc_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
void vp9_dc_predictor_8x8_sse(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
RTCD_EXTERN void (*vp9_dc_predictor_8x8)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

void vp9_dc_top_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_dc_top_predictor_8x8 vp9_dc_top_predictor_8x8_c

void vp9_dc_left_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_dc_left_predictor_8x8 vp9_dc_left_predictor_8x8_c

void vp9_dc_128_predictor_8x8_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_dc_128_predictor_8x8 vp9_dc_128_predictor_8x8_c

void vp9_d27_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_d27_predictor_16x16 vp9_d27_predictor_16x16_c

void vp9_d45_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
void vp9_d45_predictor_16x16_ssse3(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
RTCD_EXTERN void (*vp9_d45_predictor_16x16)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

void vp9_d63_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_d63_predictor_16x16 vp9_d63_predictor_16x16_c

void vp9_h_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
void vp9_h_predictor_16x16_ssse3(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
RTCD_EXTERN void (*vp9_h_predictor_16x16)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

void vp9_d117_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_d117_predictor_16x16 vp9_d117_predictor_16x16_c

void vp9_d135_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_d135_predictor_16x16 vp9_d135_predictor_16x16_c

void vp9_d153_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_d153_predictor_16x16 vp9_d153_predictor_16x16_c

void vp9_v_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
void vp9_v_predictor_16x16_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
RTCD_EXTERN void (*vp9_v_predictor_16x16)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

void vp9_tm_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
void vp9_tm_predictor_16x16_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
RTCD_EXTERN void (*vp9_tm_predictor_16x16)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

void vp9_dc_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
void vp9_dc_predictor_16x16_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
RTCD_EXTERN void (*vp9_dc_predictor_16x16)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

void vp9_dc_top_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_dc_top_predictor_16x16 vp9_dc_top_predictor_16x16_c

void vp9_dc_left_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_dc_left_predictor_16x16 vp9_dc_left_predictor_16x16_c

void vp9_dc_128_predictor_16x16_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_dc_128_predictor_16x16 vp9_dc_128_predictor_16x16_c

void vp9_d27_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_d27_predictor_32x32 vp9_d27_predictor_32x32_c

void vp9_d45_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
void vp9_d45_predictor_32x32_ssse3(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
RTCD_EXTERN void (*vp9_d45_predictor_32x32)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

void vp9_d63_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_d63_predictor_32x32 vp9_d63_predictor_32x32_c

void vp9_h_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_h_predictor_32x32 vp9_h_predictor_32x32_c

void vp9_d117_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_d117_predictor_32x32 vp9_d117_predictor_32x32_c

void vp9_d135_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_d135_predictor_32x32 vp9_d135_predictor_32x32_c

void vp9_d153_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_d153_predictor_32x32 vp9_d153_predictor_32x32_c

void vp9_v_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
void vp9_v_predictor_32x32_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
RTCD_EXTERN void (*vp9_v_predictor_32x32)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

void vp9_tm_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_tm_predictor_32x32 vp9_tm_predictor_32x32_c

void vp9_dc_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
void vp9_dc_predictor_32x32_sse2(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
RTCD_EXTERN void (*vp9_dc_predictor_32x32)(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);

void vp9_dc_top_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_dc_top_predictor_32x32 vp9_dc_top_predictor_32x32_c

void vp9_dc_left_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_dc_left_predictor_32x32 vp9_dc_left_predictor_32x32_c

void vp9_dc_128_predictor_32x32_c(uint8_t *dst, ptrdiff_t y_stride, const uint8_t *above, const uint8_t *left);
#define vp9_dc_128_predictor_32x32 vp9_dc_128_predictor_32x32_c

void vp9_add_constant_residual_8x8_c(const int16_t diff, uint8_t *dest, int stride);
void vp9_add_constant_residual_8x8_sse2(const int16_t diff, uint8_t *dest, int stride);
RTCD_EXTERN void (*vp9_add_constant_residual_8x8)(const int16_t diff, uint8_t *dest, int stride);

void vp9_add_constant_residual_16x16_c(const int16_t diff, uint8_t *dest, int stride);
void vp9_add_constant_residual_16x16_sse2(const int16_t diff, uint8_t *dest, int stride);
RTCD_EXTERN void (*vp9_add_constant_residual_16x16)(const int16_t diff, uint8_t *dest, int stride);

void vp9_add_constant_residual_32x32_c(const int16_t diff, uint8_t *dest, int stride);
void vp9_add_constant_residual_32x32_sse2(const int16_t diff, uint8_t *dest, int stride);
RTCD_EXTERN void (*vp9_add_constant_residual_32x32)(const int16_t diff, uint8_t *dest, int stride);

void vp9_mb_lpf_vertical_edge_w_c(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh);
void vp9_mb_lpf_vertical_edge_w_sse2(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh);
RTCD_EXTERN void (*vp9_mb_lpf_vertical_edge_w)(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh);

void vp9_mbloop_filter_vertical_edge_c(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh, int count);
void vp9_mbloop_filter_vertical_edge_sse2(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh, int count);
RTCD_EXTERN void (*vp9_mbloop_filter_vertical_edge)(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh, int count);

void vp9_loop_filter_vertical_edge_c(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh, int count);
void vp9_loop_filter_vertical_edge_mmx(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh, int count);
RTCD_EXTERN void (*vp9_loop_filter_vertical_edge)(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh, int count);

void vp9_mb_lpf_horizontal_edge_w_c(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh, int count);
void vp9_mb_lpf_horizontal_edge_w_sse2(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh, int count);
RTCD_EXTERN void (*vp9_mb_lpf_horizontal_edge_w)(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh, int count);

void vp9_mbloop_filter_horizontal_edge_c(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh, int count);
void vp9_mbloop_filter_horizontal_edge_sse2(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh, int count);
RTCD_EXTERN void (*vp9_mbloop_filter_horizontal_edge)(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh, int count);

void vp9_loop_filter_horizontal_edge_c(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh, int count);
void vp9_loop_filter_horizontal_edge_mmx(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh, int count);
RTCD_EXTERN void (*vp9_loop_filter_horizontal_edge)(uint8_t *s, int pitch, const uint8_t *blimit, const uint8_t *limit, const uint8_t *thresh, int count);

void vp9_mbpost_proc_down_c(uint8_t *dst, int pitch, int rows, int cols, int flimit);
void vp9_mbpost_proc_down_mmx(uint8_t *dst, int pitch, int rows, int cols, int flimit);
void vp9_mbpost_proc_down_xmm(uint8_t *dst, int pitch, int rows, int cols, int flimit);
RTCD_EXTERN void (*vp9_mbpost_proc_down)(uint8_t *dst, int pitch, int rows, int cols, int flimit);

void vp9_mbpost_proc_across_ip_c(uint8_t *src, int pitch, int rows, int cols, int flimit);
void vp9_mbpost_proc_across_ip_xmm(uint8_t *src, int pitch, int rows, int cols, int flimit);
RTCD_EXTERN void (*vp9_mbpost_proc_across_ip)(uint8_t *src, int pitch, int rows, int cols, int flimit);

void vp9_post_proc_down_and_across_c(const uint8_t *src_ptr, uint8_t *dst_ptr, int src_pixels_per_line, int dst_pixels_per_line, int rows, int cols, int flimit);
void vp9_post_proc_down_and_across_mmx(const uint8_t *src_ptr, uint8_t *dst_ptr, int src_pixels_per_line, int dst_pixels_per_line, int rows, int cols, int flimit);
void vp9_post_proc_down_and_across_xmm(const uint8_t *src_ptr, uint8_t *dst_ptr, int src_pixels_per_line, int dst_pixels_per_line, int rows, int cols, int flimit);
RTCD_EXTERN void (*vp9_post_proc_down_and_across)(const uint8_t *src_ptr, uint8_t *dst_ptr, int src_pixels_per_line, int dst_pixels_per_line, int rows, int cols, int flimit);

void vp9_plane_add_noise_c(uint8_t *Start, char *noise, char blackclamp[16], char whiteclamp[16], char bothclamp[16], unsigned int Width, unsigned int Height, int Pitch);
void vp9_plane_add_noise_mmx(uint8_t *Start, char *noise, char blackclamp[16], char whiteclamp[16], char bothclamp[16], unsigned int Width, unsigned int Height, int Pitch);
void vp9_plane_add_noise_wmt(uint8_t *Start, char *noise, char blackclamp[16], char whiteclamp[16], char bothclamp[16], unsigned int Width, unsigned int Height, int Pitch);
RTCD_EXTERN void (*vp9_plane_add_noise)(uint8_t *Start, char *noise, char blackclamp[16], char whiteclamp[16], char bothclamp[16], unsigned int Width, unsigned int Height, int Pitch);

void vp9_blend_mb_inner_c(uint8_t *y, uint8_t *u, uint8_t *v, int y1, int u1, int v1, int alpha, int stride);
#define vp9_blend_mb_inner vp9_blend_mb_inner_c

void vp9_blend_mb_outer_c(uint8_t *y, uint8_t *u, uint8_t *v, int y1, int u1, int v1, int alpha, int stride);
#define vp9_blend_mb_outer vp9_blend_mb_outer_c

void vp9_blend_b_c(uint8_t *y, uint8_t *u, uint8_t *v, int y1, int u1, int v1, int alpha, int stride);
#define vp9_blend_b vp9_blend_b_c

void vp9_convolve_copy_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h);
void vp9_convolve_copy_sse2(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h);
RTCD_EXTERN void (*vp9_convolve_copy)(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h);

void vp9_convolve_avg_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h);
void vp9_convolve_avg_sse2(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h);
RTCD_EXTERN void (*vp9_convolve_avg)(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h);

void vp9_convolve8_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h);
void vp9_convolve8_ssse3(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h);
RTCD_EXTERN void (*vp9_convolve8)(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h);

void vp9_convolve8_horiz_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h);
void vp9_convolve8_horiz_ssse3(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h);
RTCD_EXTERN void (*vp9_convolve8_horiz)(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h);

void vp9_convolve8_vert_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h);
void vp9_convolve8_vert_ssse3(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h);
RTCD_EXTERN void (*vp9_convolve8_vert)(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h);

void vp9_convolve8_avg_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h);
void vp9_convolve8_avg_ssse3(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h);
RTCD_EXTERN void (*vp9_convolve8_avg)(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h);

void vp9_convolve8_avg_horiz_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h);
void vp9_convolve8_avg_horiz_ssse3(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h);
RTCD_EXTERN void (*vp9_convolve8_avg_horiz)(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h);

void vp9_convolve8_avg_vert_c(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h);
void vp9_convolve8_avg_vert_ssse3(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h);
RTCD_EXTERN void (*vp9_convolve8_avg_vert)(const uint8_t *src, ptrdiff_t src_stride, uint8_t *dst, ptrdiff_t dst_stride, const int16_t *filter_x, int x_step_q4, const int16_t *filter_y, int y_step_q4, int w, int h);

void vp9_short_idct4x4_1_add_c(int16_t *input, uint8_t *dest, int dest_stride);
void vp9_short_idct4x4_1_add_sse2(int16_t *input, uint8_t *dest, int dest_stride);
RTCD_EXTERN void (*vp9_short_idct4x4_1_add)(int16_t *input, uint8_t *dest, int dest_stride);

void vp9_short_idct4x4_add_c(int16_t *input, uint8_t *dest, int dest_stride);
void vp9_short_idct4x4_add_sse2(int16_t *input, uint8_t *dest, int dest_stride);
RTCD_EXTERN void (*vp9_short_idct4x4_add)(int16_t *input, uint8_t *dest, int dest_stride);

void vp9_short_idct8x8_1_add_c(int16_t *input, uint8_t *dest, int dest_stride);
void vp9_short_idct8x8_1_add_sse2(int16_t *input, uint8_t *dest, int dest_stride);
RTCD_EXTERN void (*vp9_short_idct8x8_1_add)(int16_t *input, uint8_t *dest, int dest_stride);

void vp9_short_idct8x8_add_c(int16_t *input, uint8_t *dest, int dest_stride);
void vp9_short_idct8x8_add_sse2(int16_t *input, uint8_t *dest, int dest_stride);
RTCD_EXTERN void (*vp9_short_idct8x8_add)(int16_t *input, uint8_t *dest, int dest_stride);

void vp9_short_idct10_8x8_add_c(int16_t *input, uint8_t *dest, int dest_stride);
void vp9_short_idct10_8x8_add_sse2(int16_t *input, uint8_t *dest, int dest_stride);
RTCD_EXTERN void (*vp9_short_idct10_8x8_add)(int16_t *input, uint8_t *dest, int dest_stride);

void vp9_short_idct16x16_1_add_c(int16_t *input, uint8_t *dest, int dest_stride);
void vp9_short_idct16x16_1_add_sse2(int16_t *input, uint8_t *dest, int dest_stride);
RTCD_EXTERN void (*vp9_short_idct16x16_1_add)(int16_t *input, uint8_t *dest, int dest_stride);

void vp9_short_idct16x16_add_c(int16_t *input, uint8_t *dest, int dest_stride);
void vp9_short_idct16x16_add_sse2(int16_t *input, uint8_t *dest, int dest_stride);
RTCD_EXTERN void (*vp9_short_idct16x16_add)(int16_t *input, uint8_t *dest, int dest_stride);

void vp9_short_idct10_16x16_add_c(int16_t *input, uint8_t *dest, int dest_stride);
void vp9_short_idct10_16x16_add_sse2(int16_t *input, uint8_t *dest, int dest_stride);
RTCD_EXTERN void (*vp9_short_idct10_16x16_add)(int16_t *input, uint8_t *dest, int dest_stride);

void vp9_short_idct32x32_add_c(int16_t *input, uint8_t *dest, int dest_stride);
void vp9_short_idct32x32_add_sse2(int16_t *input, uint8_t *dest, int dest_stride);
RTCD_EXTERN void (*vp9_short_idct32x32_add)(int16_t *input, uint8_t *dest, int dest_stride);

void vp9_short_idct1_32x32_c(int16_t *input, int16_t *output);
#define vp9_short_idct1_32x32 vp9_short_idct1_32x32_c

void vp9_short_iht4x4_add_c(int16_t *input, uint8_t *dest, int dest_stride, int tx_type);
void vp9_short_iht4x4_add_sse2(int16_t *input, uint8_t *dest, int dest_stride, int tx_type);
RTCD_EXTERN void (*vp9_short_iht4x4_add)(int16_t *input, uint8_t *dest, int dest_stride, int tx_type);

void vp9_short_iht8x8_add_c(int16_t *input, uint8_t *dest, int dest_stride, int tx_type);
void vp9_short_iht8x8_add_sse2(int16_t *input, uint8_t *dest, int dest_stride, int tx_type);
RTCD_EXTERN void (*vp9_short_iht8x8_add)(int16_t *input, uint8_t *dest, int dest_stride, int tx_type);

void vp9_short_iht16x16_add_c(int16_t *input, uint8_t *output, int pitch, int tx_type);
void vp9_short_iht16x16_add_sse2(int16_t *input, uint8_t *output, int pitch, int tx_type);
RTCD_EXTERN void (*vp9_short_iht16x16_add)(int16_t *input, uint8_t *output, int pitch, int tx_type);

void vp9_idct4_1d_c(int16_t *input, int16_t *output);
void vp9_idct4_1d_sse2(int16_t *input, int16_t *output);
RTCD_EXTERN void (*vp9_idct4_1d)(int16_t *input, int16_t *output);

void vp9_short_iwalsh4x4_1_add_c(int16_t *input, uint8_t *dest, int dest_stride);
#define vp9_short_iwalsh4x4_1_add vp9_short_iwalsh4x4_1_add_c

void vp9_short_iwalsh4x4_add_c(int16_t *input, uint8_t *dest, int dest_stride);
#define vp9_short_iwalsh4x4_add vp9_short_iwalsh4x4_add_c

unsigned int vp9_sad32x3_c(const uint8_t *src_ptr, int  src_stride, const uint8_t *ref_ptr, int ref_stride, int max_sad);
#define vp9_sad32x3 vp9_sad32x3_c

unsigned int vp9_sad3x32_c(const uint8_t *src_ptr, int  src_stride, const uint8_t *ref_ptr, int ref_stride, int max_sad);
#define vp9_sad3x32 vp9_sad3x32_c

unsigned int vp9_variance32x16_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_variance32x16_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_variance32x16)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_variance16x32_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_variance16x32_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_variance16x32)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_variance64x32_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_variance64x32_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_variance64x32)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_variance32x64_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_variance32x64_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_variance32x64)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_variance32x32_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_variance32x32_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_variance32x32)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_variance64x64_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_variance64x64_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_variance64x64)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_variance16x16_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_variance16x16_mmx(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_variance16x16_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_variance16x16)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_variance16x8_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_variance16x8_mmx(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_variance16x8_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_variance16x8)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_variance8x16_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_variance8x16_mmx(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_variance8x16_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_variance8x16)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_variance8x8_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_variance8x8_mmx(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_variance8x8_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_variance8x8)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

void vp9_get_sse_sum_8x8_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, int *sum);
void vp9_get8x8var_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, int *sum);
RTCD_EXTERN void (*vp9_get_sse_sum_8x8)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, int *sum);

unsigned int vp9_variance8x4_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_variance8x4_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_variance8x4)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_variance4x8_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_variance4x8_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_variance4x8)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_variance4x4_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_variance4x4_mmx(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_variance4x4_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_variance4x4)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_sub_pixel_variance64x64_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance64x64_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance64x64_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_variance64x64)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_sub_pixel_avg_variance64x64_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance64x64_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance64x64_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_avg_variance64x64)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);

unsigned int vp9_sub_pixel_variance32x64_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance32x64_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance32x64_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_variance32x64)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_sub_pixel_avg_variance32x64_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance32x64_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance32x64_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_avg_variance32x64)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);

unsigned int vp9_sub_pixel_variance64x32_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance64x32_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance64x32_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_variance64x32)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_sub_pixel_avg_variance64x32_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance64x32_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance64x32_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_avg_variance64x32)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);

unsigned int vp9_sub_pixel_variance32x16_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance32x16_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance32x16_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_variance32x16)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_sub_pixel_avg_variance32x16_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance32x16_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance32x16_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_avg_variance32x16)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);

unsigned int vp9_sub_pixel_variance16x32_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance16x32_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance16x32_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_variance16x32)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_sub_pixel_avg_variance16x32_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance16x32_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance16x32_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_avg_variance16x32)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);

unsigned int vp9_sub_pixel_variance32x32_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance32x32_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance32x32_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_variance32x32)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_sub_pixel_avg_variance32x32_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance32x32_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance32x32_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_avg_variance32x32)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);

unsigned int vp9_sub_pixel_variance16x16_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance16x16_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance16x16_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_variance16x16)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_sub_pixel_avg_variance16x16_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance16x16_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance16x16_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_avg_variance16x16)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);

unsigned int vp9_sub_pixel_variance8x16_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance8x16_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance8x16_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_variance8x16)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_sub_pixel_avg_variance8x16_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance8x16_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance8x16_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_avg_variance8x16)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);

unsigned int vp9_sub_pixel_variance16x8_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance16x8_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance16x8_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_variance16x8)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_sub_pixel_avg_variance16x8_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance16x8_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance16x8_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_avg_variance16x8)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);

unsigned int vp9_sub_pixel_variance8x8_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance8x8_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance8x8_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_variance8x8)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_sub_pixel_avg_variance8x8_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance8x8_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance8x8_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_avg_variance8x8)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);

unsigned int vp9_sub_pixel_variance8x4_c(const uint8_t *src_ptr, int source_stride, int xoffset, int yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance8x4_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance8x4_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_variance8x4)(const uint8_t *src_ptr, int source_stride, int xoffset, int yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_sub_pixel_avg_variance8x4_c(const uint8_t *src_ptr, int source_stride, int xoffset, int yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance8x4_sse2(const uint8_t *src_ptr, int source_stride, int xoffset, int yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance8x4_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_avg_variance8x4)(const uint8_t *src_ptr, int source_stride, int xoffset, int yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);

unsigned int vp9_sub_pixel_variance4x8_c(const uint8_t *src_ptr, int source_stride, int xoffset, int yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance4x8_sse(const uint8_t *src_ptr, int source_stride, int xoffset, int yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance4x8_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_variance4x8)(const uint8_t *src_ptr, int source_stride, int xoffset, int yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_sub_pixel_avg_variance4x8_c(const uint8_t *src_ptr, int source_stride, int xoffset, int yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance4x8_sse(const uint8_t *src_ptr, int source_stride, int xoffset, int yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance4x8_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_avg_variance4x8)(const uint8_t *src_ptr, int source_stride, int xoffset, int yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);

unsigned int vp9_sub_pixel_variance4x4_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance4x4_sse(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_sub_pixel_variance4x4_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_variance4x4)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_sub_pixel_avg_variance4x4_c(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance4x4_sse(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
unsigned int vp9_sub_pixel_avg_variance4x4_ssse3(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);
RTCD_EXTERN unsigned int (*vp9_sub_pixel_avg_variance4x4)(const uint8_t *src_ptr, int source_stride, int xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse, const uint8_t *second_pred);

unsigned int vp9_sad64x64_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);
unsigned int vp9_sad64x64_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad64x64)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);

unsigned int vp9_sad32x64_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int max_sad);
unsigned int vp9_sad32x64_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad32x64)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int max_sad);

unsigned int vp9_sad64x32_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int max_sad);
unsigned int vp9_sad64x32_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad64x32)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int max_sad);

unsigned int vp9_sad32x16_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int max_sad);
unsigned int vp9_sad32x16_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad32x16)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int max_sad);

unsigned int vp9_sad16x32_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int max_sad);
unsigned int vp9_sad16x32_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad16x32)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int max_sad);

unsigned int vp9_sad32x32_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);
unsigned int vp9_sad32x32_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad32x32)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);

unsigned int vp9_sad16x16_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);
unsigned int vp9_sad16x16_mmx(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);
unsigned int vp9_sad16x16_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad16x16)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);

unsigned int vp9_sad16x8_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);
unsigned int vp9_sad16x8_mmx(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);
unsigned int vp9_sad16x8_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad16x8)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);

unsigned int vp9_sad8x16_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);
unsigned int vp9_sad8x16_mmx(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);
unsigned int vp9_sad8x16_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad8x16)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);

unsigned int vp9_sad8x8_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);
unsigned int vp9_sad8x8_mmx(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);
unsigned int vp9_sad8x8_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad8x8)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);

unsigned int vp9_sad8x4_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int max_sad);
unsigned int vp9_sad8x4_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad8x4)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int max_sad);

unsigned int vp9_sad4x8_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int max_sad);
unsigned int vp9_sad4x8_sse(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad4x8)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int max_sad);

unsigned int vp9_sad4x4_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);
unsigned int vp9_sad4x4_mmx(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);
unsigned int vp9_sad4x4_sse(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad4x4)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int max_sad);

unsigned int vp9_sad64x64_avg_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, const uint8_t *second_pred, unsigned int max_sad);
unsigned int vp9_sad64x64_avg_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, const uint8_t *second_pred, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad64x64_avg)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, const uint8_t *second_pred, unsigned int max_sad);

unsigned int vp9_sad32x64_avg_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred, unsigned int max_sad);
unsigned int vp9_sad32x64_avg_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad32x64_avg)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred, unsigned int max_sad);

unsigned int vp9_sad64x32_avg_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred, unsigned int max_sad);
unsigned int vp9_sad64x32_avg_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad64x32_avg)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred, unsigned int max_sad);

unsigned int vp9_sad32x16_avg_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred, unsigned int max_sad);
unsigned int vp9_sad32x16_avg_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad32x16_avg)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred, unsigned int max_sad);

unsigned int vp9_sad16x32_avg_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred, unsigned int max_sad);
unsigned int vp9_sad16x32_avg_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad16x32_avg)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred, unsigned int max_sad);

unsigned int vp9_sad32x32_avg_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, const uint8_t *second_pred, unsigned int max_sad);
unsigned int vp9_sad32x32_avg_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, const uint8_t *second_pred, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad32x32_avg)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, const uint8_t *second_pred, unsigned int max_sad);

unsigned int vp9_sad16x16_avg_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, const uint8_t *second_pred, unsigned int max_sad);
unsigned int vp9_sad16x16_avg_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, const uint8_t *second_pred, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad16x16_avg)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, const uint8_t *second_pred, unsigned int max_sad);

unsigned int vp9_sad16x8_avg_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, const uint8_t *second_pred, unsigned int max_sad);
unsigned int vp9_sad16x8_avg_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, const uint8_t *second_pred, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad16x8_avg)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, const uint8_t *second_pred, unsigned int max_sad);

unsigned int vp9_sad8x16_avg_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, const uint8_t *second_pred, unsigned int max_sad);
unsigned int vp9_sad8x16_avg_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, const uint8_t *second_pred, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad8x16_avg)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, const uint8_t *second_pred, unsigned int max_sad);

unsigned int vp9_sad8x8_avg_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, const uint8_t *second_pred, unsigned int max_sad);
unsigned int vp9_sad8x8_avg_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, const uint8_t *second_pred, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad8x8_avg)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, const uint8_t *second_pred, unsigned int max_sad);

unsigned int vp9_sad8x4_avg_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred, unsigned int max_sad);
unsigned int vp9_sad8x4_avg_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad8x4_avg)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred, unsigned int max_sad);

unsigned int vp9_sad4x8_avg_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred, unsigned int max_sad);
unsigned int vp9_sad4x8_avg_sse(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad4x8_avg)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, const uint8_t *second_pred, unsigned int max_sad);

unsigned int vp9_sad4x4_avg_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, const uint8_t *second_pred, unsigned int max_sad);
unsigned int vp9_sad4x4_avg_sse(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, const uint8_t *second_pred, unsigned int max_sad);
RTCD_EXTERN unsigned int (*vp9_sad4x4_avg)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, const uint8_t *second_pred, unsigned int max_sad);

unsigned int vp9_variance_halfpixvar16x16_h_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_variance_halfpixvar16x16_h_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_variance_halfpixvar16x16_h)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_variance_halfpixvar16x16_v_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_variance_halfpixvar16x16_v_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_variance_halfpixvar16x16_v)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_variance_halfpixvar16x16_hv_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
unsigned int vp9_variance_halfpixvar16x16_hv_sse2(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_variance_halfpixvar16x16_hv)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);

unsigned int vp9_variance_halfpixvar64x64_h_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
#define vp9_variance_halfpixvar64x64_h vp9_variance_halfpixvar64x64_h_c

unsigned int vp9_variance_halfpixvar64x64_v_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
#define vp9_variance_halfpixvar64x64_v vp9_variance_halfpixvar64x64_v_c

unsigned int vp9_variance_halfpixvar64x64_hv_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
#define vp9_variance_halfpixvar64x64_hv vp9_variance_halfpixvar64x64_hv_c

unsigned int vp9_variance_halfpixvar32x32_h_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
#define vp9_variance_halfpixvar32x32_h vp9_variance_halfpixvar32x32_h_c

unsigned int vp9_variance_halfpixvar32x32_v_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
#define vp9_variance_halfpixvar32x32_v vp9_variance_halfpixvar32x32_v_c

unsigned int vp9_variance_halfpixvar32x32_hv_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
#define vp9_variance_halfpixvar32x32_hv vp9_variance_halfpixvar32x32_hv_c

void vp9_sad64x64x3_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int *sad_array);
#define vp9_sad64x64x3 vp9_sad64x64x3_c

void vp9_sad32x32x3_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int *sad_array);
#define vp9_sad32x32x3 vp9_sad32x32x3_c

void vp9_sad16x16x3_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int *sad_array);
void vp9_sad16x16x3_sse3(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int *sad_array);
void vp9_sad16x16x3_ssse3(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int *sad_array);
RTCD_EXTERN void (*vp9_sad16x16x3)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int *sad_array);

void vp9_sad16x8x3_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int *sad_array);
void vp9_sad16x8x3_sse3(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int *sad_array);
void vp9_sad16x8x3_ssse3(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int *sad_array);
RTCD_EXTERN void (*vp9_sad16x8x3)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int *sad_array);

void vp9_sad8x16x3_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int *sad_array);
void vp9_sad8x16x3_sse3(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int *sad_array);
RTCD_EXTERN void (*vp9_sad8x16x3)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int *sad_array);

void vp9_sad8x8x3_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int *sad_array);
void vp9_sad8x8x3_sse3(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int *sad_array);
RTCD_EXTERN void (*vp9_sad8x8x3)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int *sad_array);

void vp9_sad4x4x3_c(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int *sad_array);
void vp9_sad4x4x3_sse3(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int *sad_array);
RTCD_EXTERN void (*vp9_sad4x4x3)(const uint8_t *src_ptr, int source_stride, const uint8_t *ref_ptr, int  ref_stride, unsigned int *sad_array);

void vp9_sad64x64x8_c(const uint8_t *src_ptr, int  src_stride, const uint8_t *ref_ptr, int  ref_stride, uint32_t *sad_array);
#define vp9_sad64x64x8 vp9_sad64x64x8_c

void vp9_sad32x32x8_c(const uint8_t *src_ptr, int  src_stride, const uint8_t *ref_ptr, int  ref_stride, uint32_t *sad_array);
#define vp9_sad32x32x8 vp9_sad32x32x8_c

void vp9_sad16x16x8_c(const uint8_t *src_ptr, int  src_stride, const uint8_t *ref_ptr, int  ref_stride, uint32_t *sad_array);
#define vp9_sad16x16x8 vp9_sad16x16x8_c

void vp9_sad16x8x8_c(const uint8_t *src_ptr, int  src_stride, const uint8_t *ref_ptr, int  ref_stride, uint32_t *sad_array);
#define vp9_sad16x8x8 vp9_sad16x8x8_c

void vp9_sad8x16x8_c(const uint8_t *src_ptr, int  src_stride, const uint8_t *ref_ptr, int  ref_stride, uint32_t *sad_array);
#define vp9_sad8x16x8 vp9_sad8x16x8_c

void vp9_sad8x8x8_c(const uint8_t *src_ptr, int  src_stride, const uint8_t *ref_ptr, int  ref_stride, uint32_t *sad_array);
#define vp9_sad8x8x8 vp9_sad8x8x8_c

void vp9_sad8x4x8_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);
#define vp9_sad8x4x8 vp9_sad8x4x8_c

void vp9_sad4x8x8_c(const uint8_t *src_ptr, int src_stride, const uint8_t *ref_ptr, int ref_stride, uint32_t *sad_array);
#define vp9_sad4x8x8 vp9_sad4x8x8_c

void vp9_sad4x4x8_c(const uint8_t *src_ptr, int  src_stride, const uint8_t *ref_ptr, int  ref_stride, uint32_t *sad_array);
#define vp9_sad4x4x8 vp9_sad4x4x8_c

void vp9_sad64x64x4d_c(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);
void vp9_sad64x64x4d_sse2(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);
RTCD_EXTERN void (*vp9_sad64x64x4d)(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);

void vp9_sad32x64x4d_c(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);
void vp9_sad32x64x4d_sse2(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);
RTCD_EXTERN void (*vp9_sad32x64x4d)(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);

void vp9_sad64x32x4d_c(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);
void vp9_sad64x32x4d_sse2(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);
RTCD_EXTERN void (*vp9_sad64x32x4d)(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);

void vp9_sad32x16x4d_c(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);
void vp9_sad32x16x4d_sse2(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);
RTCD_EXTERN void (*vp9_sad32x16x4d)(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);

void vp9_sad16x32x4d_c(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);
void vp9_sad16x32x4d_sse2(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);
RTCD_EXTERN void (*vp9_sad16x32x4d)(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);

void vp9_sad32x32x4d_c(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);
void vp9_sad32x32x4d_sse2(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);
RTCD_EXTERN void (*vp9_sad32x32x4d)(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);

void vp9_sad16x16x4d_c(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);
void vp9_sad16x16x4d_sse2(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);
RTCD_EXTERN void (*vp9_sad16x16x4d)(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);

void vp9_sad16x8x4d_c(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);
void vp9_sad16x8x4d_sse2(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);
RTCD_EXTERN void (*vp9_sad16x8x4d)(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);

void vp9_sad8x16x4d_c(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);
void vp9_sad8x16x4d_sse2(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);
RTCD_EXTERN void (*vp9_sad8x16x4d)(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);

void vp9_sad8x8x4d_c(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);
void vp9_sad8x8x4d_sse2(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);
RTCD_EXTERN void (*vp9_sad8x8x4d)(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);

void vp9_sad8x4x4d_c(const uint8_t *src_ptr, int src_stride, const uint8_t* const ref_ptr[], int ref_stride, unsigned int *sad_array);
void vp9_sad8x4x4d_sse2(const uint8_t *src_ptr, int src_stride, const uint8_t* const ref_ptr[], int ref_stride, unsigned int *sad_array);
RTCD_EXTERN void (*vp9_sad8x4x4d)(const uint8_t *src_ptr, int src_stride, const uint8_t* const ref_ptr[], int ref_stride, unsigned int *sad_array);

void vp9_sad4x8x4d_c(const uint8_t *src_ptr, int src_stride, const uint8_t* const ref_ptr[], int ref_stride, unsigned int *sad_array);
void vp9_sad4x8x4d_sse(const uint8_t *src_ptr, int src_stride, const uint8_t* const ref_ptr[], int ref_stride, unsigned int *sad_array);
RTCD_EXTERN void (*vp9_sad4x8x4d)(const uint8_t *src_ptr, int src_stride, const uint8_t* const ref_ptr[], int ref_stride, unsigned int *sad_array);

void vp9_sad4x4x4d_c(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);
void vp9_sad4x4x4d_sse(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);
RTCD_EXTERN void (*vp9_sad4x4x4d)(const uint8_t *src_ptr, int  src_stride, const uint8_t* const ref_ptr[], int  ref_stride, unsigned int *sad_array);

unsigned int vp9_mse16x16_c(const uint8_t *src_ptr, int  source_stride, const uint8_t *ref_ptr, int  recon_stride, unsigned int *sse);
unsigned int vp9_mse16x16_mmx(const uint8_t *src_ptr, int  source_stride, const uint8_t *ref_ptr, int  recon_stride, unsigned int *sse);
unsigned int vp9_mse16x16_sse2(const uint8_t *src_ptr, int  source_stride, const uint8_t *ref_ptr, int  recon_stride, unsigned int *sse);
RTCD_EXTERN unsigned int (*vp9_mse16x16)(const uint8_t *src_ptr, int  source_stride, const uint8_t *ref_ptr, int  recon_stride, unsigned int *sse);

unsigned int vp9_mse8x16_c(const uint8_t *src_ptr, int  source_stride, const uint8_t *ref_ptr, int  recon_stride, unsigned int *sse);
#define vp9_mse8x16 vp9_mse8x16_c

unsigned int vp9_mse16x8_c(const uint8_t *src_ptr, int  source_stride, const uint8_t *ref_ptr, int  recon_stride, unsigned int *sse);
#define vp9_mse16x8 vp9_mse16x8_c

unsigned int vp9_mse8x8_c(const uint8_t *src_ptr, int  source_stride, const uint8_t *ref_ptr, int  recon_stride, unsigned int *sse);
#define vp9_mse8x8 vp9_mse8x8_c

unsigned int vp9_sub_pixel_mse64x64_c(const uint8_t *src_ptr, int  source_stride, int  xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
#define vp9_sub_pixel_mse64x64 vp9_sub_pixel_mse64x64_c

unsigned int vp9_sub_pixel_mse32x32_c(const uint8_t *src_ptr, int  source_stride, int  xoffset, int  yoffset, const uint8_t *ref_ptr, int ref_stride, unsigned int *sse);
#define vp9_sub_pixel_mse32x32 vp9_sub_pixel_mse32x32_c

unsigned int vp9_get_mb_ss_c(const int16_t *);
unsigned int vp9_get_mb_ss_mmx(const int16_t *);
unsigned int vp9_get_mb_ss_sse2(const int16_t *);
RTCD_EXTERN unsigned int (*vp9_get_mb_ss)(const int16_t *);

int64_t vp9_block_error_c(int16_t *coeff, int16_t *dqcoeff, intptr_t block_size, int64_t *ssz);
int64_t vp9_block_error_sse2(int16_t *coeff, int16_t *dqcoeff, intptr_t block_size, int64_t *ssz);
RTCD_EXTERN int64_t (*vp9_block_error)(int16_t *coeff, int16_t *dqcoeff, intptr_t block_size, int64_t *ssz);

void vp9_subtract_block_c(int rows, int cols, int16_t *diff_ptr, ptrdiff_t diff_stride, const uint8_t *src_ptr, ptrdiff_t src_stride, const uint8_t *pred_ptr, ptrdiff_t pred_stride);
void vp9_subtract_block_sse2(int rows, int cols, int16_t *diff_ptr, ptrdiff_t diff_stride, const uint8_t *src_ptr, ptrdiff_t src_stride, const uint8_t *pred_ptr, ptrdiff_t pred_stride);
RTCD_EXTERN void (*vp9_subtract_block)(int rows, int cols, int16_t *diff_ptr, ptrdiff_t diff_stride, const uint8_t *src_ptr, ptrdiff_t src_stride, const uint8_t *pred_ptr, ptrdiff_t pred_stride);

void vp9_quantize_b_c(int16_t *coeff_ptr, intptr_t n_coeffs, int skip_block, int16_t *zbin_ptr, int16_t *round_ptr, int16_t *quant_ptr, int16_t *quant_shift_ptr, int16_t *qcoeff_ptr, int16_t *dqcoeff_ptr, int16_t *dequant_ptr, int zbin_oq_value, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
#define vp9_quantize_b vp9_quantize_b_c

void vp9_quantize_b_32x32_c(int16_t *coeff_ptr, intptr_t n_coeffs, int skip_block, int16_t *zbin_ptr, int16_t *round_ptr, int16_t *quant_ptr, int16_t *quant_shift_ptr, int16_t *qcoeff_ptr, int16_t *dqcoeff_ptr, int16_t *dequant_ptr, int zbin_oq_value, uint16_t *eob_ptr, const int16_t *scan, const int16_t *iscan);
#define vp9_quantize_b_32x32 vp9_quantize_b_32x32_c

void vp9_short_fht4x4_c(int16_t *InputData, int16_t *OutputData, int pitch, int tx_type);
void vp9_short_fht4x4_sse2(int16_t *InputData, int16_t *OutputData, int pitch, int tx_type);
RTCD_EXTERN void (*vp9_short_fht4x4)(int16_t *InputData, int16_t *OutputData, int pitch, int tx_type);

void vp9_short_fht8x8_c(int16_t *InputData, int16_t *OutputData, int pitch, int tx_type);
void vp9_short_fht8x8_sse2(int16_t *InputData, int16_t *OutputData, int pitch, int tx_type);
RTCD_EXTERN void (*vp9_short_fht8x8)(int16_t *InputData, int16_t *OutputData, int pitch, int tx_type);

void vp9_short_fht16x16_c(int16_t *InputData, int16_t *OutputData, int pitch, int tx_type);
void vp9_short_fht16x16_sse2(int16_t *InputData, int16_t *OutputData, int pitch, int tx_type);
RTCD_EXTERN void (*vp9_short_fht16x16)(int16_t *InputData, int16_t *OutputData, int pitch, int tx_type);

void vp9_short_fdct8x8_c(int16_t *InputData, int16_t *OutputData, int pitch);
void vp9_short_fdct8x8_sse2(int16_t *InputData, int16_t *OutputData, int pitch);
RTCD_EXTERN void (*vp9_short_fdct8x8)(int16_t *InputData, int16_t *OutputData, int pitch);

void vp9_short_fdct4x4_c(int16_t *InputData, int16_t *OutputData, int pitch);
void vp9_short_fdct4x4_sse2(int16_t *InputData, int16_t *OutputData, int pitch);
RTCD_EXTERN void (*vp9_short_fdct4x4)(int16_t *InputData, int16_t *OutputData, int pitch);

void vp9_short_fdct8x4_c(int16_t *InputData, int16_t *OutputData, int pitch);
void vp9_short_fdct8x4_sse2(int16_t *InputData, int16_t *OutputData, int pitch);
RTCD_EXTERN void (*vp9_short_fdct8x4)(int16_t *InputData, int16_t *OutputData, int pitch);

void vp9_short_fdct32x32_c(int16_t *InputData, int16_t *OutputData, int pitch);
void vp9_short_fdct32x32_sse2(int16_t *InputData, int16_t *OutputData, int pitch);
RTCD_EXTERN void (*vp9_short_fdct32x32)(int16_t *InputData, int16_t *OutputData, int pitch);

void vp9_short_fdct32x32_rd_c(int16_t *InputData, int16_t *OutputData, int pitch);
void vp9_short_fdct32x32_rd_sse2(int16_t *InputData, int16_t *OutputData, int pitch);
RTCD_EXTERN void (*vp9_short_fdct32x32_rd)(int16_t *InputData, int16_t *OutputData, int pitch);

void vp9_short_fdct16x16_c(int16_t *InputData, int16_t *OutputData, int pitch);
void vp9_short_fdct16x16_sse2(int16_t *InputData, int16_t *OutputData, int pitch);
RTCD_EXTERN void (*vp9_short_fdct16x16)(int16_t *InputData, int16_t *OutputData, int pitch);

void vp9_short_walsh4x4_c(int16_t *InputData, int16_t *OutputData, int pitch);
#define vp9_short_walsh4x4 vp9_short_walsh4x4_c

void vp9_short_walsh8x4_c(int16_t *InputData, int16_t *OutputData, int pitch);
#define vp9_short_walsh8x4 vp9_short_walsh8x4_c

int vp9_full_search_sad_c(struct macroblock *x, union int_mv *ref_mv, int sad_per_bit, int distance, struct vp9_variance_vtable *fn_ptr, DEC_MVCOSTS, union int_mv *center_mv, int n);
int vp9_full_search_sadx3(struct macroblock *x, union int_mv *ref_mv, int sad_per_bit, int distance, struct vp9_variance_vtable *fn_ptr, DEC_MVCOSTS, union int_mv *center_mv, int n);
int vp9_full_search_sadx8(struct macroblock *x, union int_mv *ref_mv, int sad_per_bit, int distance, struct vp9_variance_vtable *fn_ptr, DEC_MVCOSTS, union int_mv *center_mv, int n);
RTCD_EXTERN int (*vp9_full_search_sad)(struct macroblock *x, union int_mv *ref_mv, int sad_per_bit, int distance, struct vp9_variance_vtable *fn_ptr, DEC_MVCOSTS, union int_mv *center_mv, int n);

int vp9_refining_search_sad_c(struct macroblock *x, union int_mv *ref_mv, int sad_per_bit, int distance, struct vp9_variance_vtable *fn_ptr, DEC_MVCOSTS, union int_mv *center_mv);
int vp9_refining_search_sadx4(struct macroblock *x, union int_mv *ref_mv, int sad_per_bit, int distance, struct vp9_variance_vtable *fn_ptr, DEC_MVCOSTS, union int_mv *center_mv);
RTCD_EXTERN int (*vp9_refining_search_sad)(struct macroblock *x, union int_mv *ref_mv, int sad_per_bit, int distance, struct vp9_variance_vtable *fn_ptr, DEC_MVCOSTS, union int_mv *center_mv);

int vp9_diamond_search_sad_c(struct macroblock *x, union int_mv *ref_mv, union int_mv *best_mv, int search_param, int sad_per_bit, int *num00, struct vp9_variance_vtable *fn_ptr, DEC_MVCOSTS, union int_mv *center_mv);
int vp9_diamond_search_sadx4(struct macroblock *x, union int_mv *ref_mv, union int_mv *best_mv, int search_param, int sad_per_bit, int *num00, struct vp9_variance_vtable *fn_ptr, DEC_MVCOSTS, union int_mv *center_mv);
RTCD_EXTERN int (*vp9_diamond_search_sad)(struct macroblock *x, union int_mv *ref_mv, union int_mv *best_mv, int search_param, int sad_per_bit, int *num00, struct vp9_variance_vtable *fn_ptr, DEC_MVCOSTS, union int_mv *center_mv);

void vp9_temporal_filter_apply_c(uint8_t *frame1, unsigned int stride, uint8_t *frame2, unsigned int block_size, int strength, int filter_weight, unsigned int *accumulator, uint16_t *count);
void vp9_temporal_filter_apply_sse2(uint8_t *frame1, unsigned int stride, uint8_t *frame2, unsigned int block_size, int strength, int filter_weight, unsigned int *accumulator, uint16_t *count);
RTCD_EXTERN void (*vp9_temporal_filter_apply)(uint8_t *frame1, unsigned int stride, uint8_t *frame2, unsigned int block_size, int strength, int filter_weight, unsigned int *accumulator, uint16_t *count);

void vp9_yv12_copy_partial_frame_c(struct yv12_buffer_config *src_ybc, struct yv12_buffer_config *dst_ybc, int fraction);
#define vp9_yv12_copy_partial_frame vp9_yv12_copy_partial_frame_c

void vp9_rtcd(void);

#ifdef RTCD_C
#include "vpx_ports/x86.h"
static void setup_rtcd_internal(void)
{
    int flags = x86_simd_caps();

    (void)flags;






    vp9_d45_predictor_4x4 = vp9_d45_predictor_4x4_c;
    //if (flags & HAS_SSSE3) vp9_d45_predictor_4x4 = vp9_d45_predictor_4x4_ssse3;


    vp9_h_predictor_4x4 = vp9_h_predictor_4x4_c;
    //if (flags & HAS_SSSE3) vp9_h_predictor_4x4 = vp9_h_predictor_4x4_ssse3;




    vp9_v_predictor_4x4 = vp9_v_predictor_4x4_c;
    //if (flags & HAS_SSE) vp9_v_predictor_4x4 = vp9_v_predictor_4x4_sse;

    vp9_tm_predictor_4x4 = vp9_tm_predictor_4x4_c;
    //if (flags & HAS_SSE) vp9_tm_predictor_4x4 = vp9_tm_predictor_4x4_sse;

    vp9_dc_predictor_4x4 = vp9_dc_predictor_4x4_c;
    //if (flags & HAS_SSE) vp9_dc_predictor_4x4 = vp9_dc_predictor_4x4_sse;





    vp9_d45_predictor_8x8 = vp9_d45_predictor_8x8_c;
    //if (flags & HAS_SSSE3) vp9_d45_predictor_8x8 = vp9_d45_predictor_8x8_ssse3;


    vp9_h_predictor_8x8 = vp9_h_predictor_8x8_c;
    //if (flags & HAS_SSSE3) vp9_h_predictor_8x8 = vp9_h_predictor_8x8_ssse3;




    vp9_v_predictor_8x8 = vp9_v_predictor_8x8_c;
    //if (flags & HAS_SSE) vp9_v_predictor_8x8 = vp9_v_predictor_8x8_sse;

    vp9_tm_predictor_8x8 = vp9_tm_predictor_8x8_c;
    //if (flags & HAS_SSE2) vp9_tm_predictor_8x8 = vp9_tm_predictor_8x8_sse2;

    vp9_dc_predictor_8x8 = vp9_dc_predictor_8x8_c;
    //if (flags & HAS_SSE) vp9_dc_predictor_8x8 = vp9_dc_predictor_8x8_sse;





    vp9_d45_predictor_16x16 = vp9_d45_predictor_16x16_c;
    //if (flags & HAS_SSSE3) vp9_d45_predictor_16x16 = vp9_d45_predictor_16x16_ssse3;


    vp9_h_predictor_16x16 = vp9_h_predictor_16x16_c;
    //if (flags & HAS_SSSE3) vp9_h_predictor_16x16 = vp9_h_predictor_16x16_ssse3;




    vp9_v_predictor_16x16 = vp9_v_predictor_16x16_c;
    //if (flags & HAS_SSE2) vp9_v_predictor_16x16 = vp9_v_predictor_16x16_sse2;

    vp9_tm_predictor_16x16 = vp9_tm_predictor_16x16_c;
    //if (flags & HAS_SSE2) vp9_tm_predictor_16x16 = vp9_tm_predictor_16x16_sse2;

    vp9_dc_predictor_16x16 = vp9_dc_predictor_16x16_c;
    //if (flags & HAS_SSE2) vp9_dc_predictor_16x16 = vp9_dc_predictor_16x16_sse2;





    vp9_d45_predictor_32x32 = vp9_d45_predictor_32x32_c;
    //if (flags & HAS_SSSE3) vp9_d45_predictor_32x32 = vp9_d45_predictor_32x32_ssse3;






    vp9_v_predictor_32x32 = vp9_v_predictor_32x32_c;
    //if (flags & HAS_SSE2) vp9_v_predictor_32x32 = vp9_v_predictor_32x32_sse2;


    vp9_dc_predictor_32x32 = vp9_dc_predictor_32x32_c;
    //if (flags & HAS_SSE2) vp9_dc_predictor_32x32 = vp9_dc_predictor_32x32_sse2;




    vp9_add_constant_residual_8x8 = vp9_add_constant_residual_8x8_c;
    //if (flags & HAS_SSE2) vp9_add_constant_residual_8x8 = vp9_add_constant_residual_8x8_sse2;

    vp9_add_constant_residual_16x16 = vp9_add_constant_residual_16x16_c;
    //if (flags & HAS_SSE2) vp9_add_constant_residual_16x16 = vp9_add_constant_residual_16x16_sse2;

    vp9_add_constant_residual_32x32 = vp9_add_constant_residual_32x32_c;
    //if (flags & HAS_SSE2) vp9_add_constant_residual_32x32 = vp9_add_constant_residual_32x32_sse2;

    vp9_mb_lpf_vertical_edge_w = vp9_mb_lpf_vertical_edge_w_c;
    //if (flags & HAS_SSE2) vp9_mb_lpf_vertical_edge_w = vp9_mb_lpf_vertical_edge_w_sse2;

    vp9_mbloop_filter_vertical_edge = vp9_mbloop_filter_vertical_edge_c;
    //if (flags & HAS_SSE2) vp9_mbloop_filter_vertical_edge = vp9_mbloop_filter_vertical_edge_sse2;

    vp9_loop_filter_vertical_edge = vp9_loop_filter_vertical_edge_c;
    //if (flags & HAS_MMX) vp9_loop_filter_vertical_edge = vp9_loop_filter_vertical_edge_mmx;

    vp9_mb_lpf_horizontal_edge_w = vp9_mb_lpf_horizontal_edge_w_c;
    //if (flags & HAS_SSE2) vp9_mb_lpf_horizontal_edge_w = vp9_mb_lpf_horizontal_edge_w_sse2;

    vp9_mbloop_filter_horizontal_edge = vp9_mbloop_filter_horizontal_edge_c;
    //if (flags & HAS_SSE2) vp9_mbloop_filter_horizontal_edge = vp9_mbloop_filter_horizontal_edge_sse2;

    vp9_loop_filter_horizontal_edge = vp9_loop_filter_horizontal_edge_c;
    //if (flags & HAS_MMX) vp9_loop_filter_horizontal_edge = vp9_loop_filter_horizontal_edge_mmx;

    vp9_mbpost_proc_down = vp9_mbpost_proc_down_c;
    //if (flags & HAS_MMX) vp9_mbpost_proc_down = vp9_mbpost_proc_down_mmx;
    //if (flags & HAS_SSE2) vp9_mbpost_proc_down = vp9_mbpost_proc_down_xmm;

    vp9_mbpost_proc_across_ip = vp9_mbpost_proc_across_ip_c;
    //if (flags & HAS_SSE2) vp9_mbpost_proc_across_ip = vp9_mbpost_proc_across_ip_xmm;

    vp9_post_proc_down_and_across = vp9_post_proc_down_and_across_c;
    //if (flags & HAS_MMX) vp9_post_proc_down_and_across = vp9_post_proc_down_and_across_mmx;
    //if (flags & HAS_SSE2) vp9_post_proc_down_and_across = vp9_post_proc_down_and_across_xmm;

    vp9_plane_add_noise = vp9_plane_add_noise_c;
    //if (flags & HAS_MMX) vp9_plane_add_noise = vp9_plane_add_noise_mmx;
    //if (flags & HAS_SSE2) vp9_plane_add_noise = vp9_plane_add_noise_wmt;




    vp9_convolve_copy = vp9_convolve_copy_c;
    //if (flags & HAS_SSE2) vp9_convolve_copy = vp9_convolve_copy_sse2;

    vp9_convolve_avg = vp9_convolve_avg_c;
    //if (flags & HAS_SSE2) vp9_convolve_avg = vp9_convolve_avg_sse2;

    vp9_convolve8 = vp9_convolve8_c;
    //if (flags & HAS_SSSE3) vp9_convolve8 = vp9_convolve8_ssse3;

    vp9_convolve8_horiz = vp9_convolve8_horiz_c;
    //if (flags & HAS_SSSE3) vp9_convolve8_horiz = vp9_convolve8_horiz_ssse3;

    vp9_convolve8_vert = vp9_convolve8_vert_c;
    //if (flags & HAS_SSSE3) vp9_convolve8_vert = vp9_convolve8_vert_ssse3;

    vp9_convolve8_avg = vp9_convolve8_avg_c;
    //if (flags & HAS_SSSE3) vp9_convolve8_avg = vp9_convolve8_avg_ssse3;

    vp9_convolve8_avg_horiz = vp9_convolve8_avg_horiz_c;
    //if (flags & HAS_SSSE3) vp9_convolve8_avg_horiz = vp9_convolve8_avg_horiz_ssse3;

    vp9_convolve8_avg_vert = vp9_convolve8_avg_vert_c;
    //if (flags & HAS_SSSE3) vp9_convolve8_avg_vert = vp9_convolve8_avg_vert_ssse3;

    vp9_short_idct4x4_1_add = vp9_short_idct4x4_1_add_c;
    //if (flags & HAS_SSE2) vp9_short_idct4x4_1_add = vp9_short_idct4x4_1_add_sse2;

    vp9_short_idct4x4_add = vp9_short_idct4x4_add_c;
    //if (flags & HAS_SSE2) vp9_short_idct4x4_add = vp9_short_idct4x4_add_sse2;

    vp9_short_idct8x8_1_add = vp9_short_idct8x8_1_add_c;
    //if (flags & HAS_SSE2) vp9_short_idct8x8_1_add = vp9_short_idct8x8_1_add_sse2;

    vp9_short_idct8x8_add = vp9_short_idct8x8_add_c;
    //if (flags & HAS_SSE2) vp9_short_idct8x8_add = vp9_short_idct8x8_add_sse2;

    vp9_short_idct10_8x8_add = vp9_short_idct10_8x8_add_c;
    //if (flags & HAS_SSE2) vp9_short_idct10_8x8_add = vp9_short_idct10_8x8_add_sse2;

    vp9_short_idct16x16_1_add = vp9_short_idct16x16_1_add_c;
    //if (flags & HAS_SSE2) vp9_short_idct16x16_1_add = vp9_short_idct16x16_1_add_sse2;

    vp9_short_idct16x16_add = vp9_short_idct16x16_add_c;
    //if (flags & HAS_SSE2) vp9_short_idct16x16_add = vp9_short_idct16x16_add_sse2;

    vp9_short_idct10_16x16_add = vp9_short_idct10_16x16_add_c;
    //if (flags & HAS_SSE2) vp9_short_idct10_16x16_add = vp9_short_idct10_16x16_add_sse2;

    vp9_short_idct32x32_add = vp9_short_idct32x32_add_c;
    //if (flags & HAS_SSE2) vp9_short_idct32x32_add = vp9_short_idct32x32_add_sse2;


    vp9_short_iht4x4_add = vp9_short_iht4x4_add_c;
    //if (flags & HAS_SSE2) vp9_short_iht4x4_add = vp9_short_iht4x4_add_sse2;

    vp9_short_iht8x8_add = vp9_short_iht8x8_add_c;
    //if (flags & HAS_SSE2) vp9_short_iht8x8_add = vp9_short_iht8x8_add_sse2;

    vp9_short_iht16x16_add = vp9_short_iht16x16_add_c;
    //if (flags & HAS_SSE2) vp9_short_iht16x16_add = vp9_short_iht16x16_add_sse2;

    vp9_idct4_1d = vp9_idct4_1d_c;
    //if (flags & HAS_SSE2) vp9_idct4_1d = vp9_idct4_1d_sse2;





    vp9_variance32x16 = vp9_variance32x16_c;
    //if (flags & HAS_SSE2) vp9_variance32x16 = vp9_variance32x16_sse2;

    vp9_variance16x32 = vp9_variance16x32_c;
    //if (flags & HAS_SSE2) vp9_variance16x32 = vp9_variance16x32_sse2;

    vp9_variance64x32 = vp9_variance64x32_c;
    //if (flags & HAS_SSE2) vp9_variance64x32 = vp9_variance64x32_sse2;

    vp9_variance32x64 = vp9_variance32x64_c;
    //if (flags & HAS_SSE2) vp9_variance32x64 = vp9_variance32x64_sse2;

    vp9_variance32x32 = vp9_variance32x32_c;
    //if (flags & HAS_SSE2) vp9_variance32x32 = vp9_variance32x32_sse2;

    vp9_variance64x64 = vp9_variance64x64_c;
    //if (flags & HAS_SSE2) vp9_variance64x64 = vp9_variance64x64_sse2;

    vp9_variance16x16 = vp9_variance16x16_c;
    //if (flags & HAS_MMX) vp9_variance16x16 = vp9_variance16x16_mmx;
    //if (flags & HAS_SSE2) vp9_variance16x16 = vp9_variance16x16_sse2;

    vp9_variance16x8 = vp9_variance16x8_c;
    //if (flags & HAS_MMX) vp9_variance16x8 = vp9_variance16x8_mmx;
    //if (flags & HAS_SSE2) vp9_variance16x8 = vp9_variance16x8_sse2;

    vp9_variance8x16 = vp9_variance8x16_c;
    //if (flags & HAS_MMX) vp9_variance8x16 = vp9_variance8x16_mmx;
    //if (flags & HAS_SSE2) vp9_variance8x16 = vp9_variance8x16_sse2;

    vp9_variance8x8 = vp9_variance8x8_c;
    //if (flags & HAS_MMX) vp9_variance8x8 = vp9_variance8x8_mmx;
    //if (flags & HAS_SSE2) vp9_variance8x8 = vp9_variance8x8_sse2;

    vp9_get_sse_sum_8x8 = vp9_get_sse_sum_8x8_c;
    //if (flags & HAS_SSE2) vp9_get_sse_sum_8x8 = vp9_get8x8var_sse2;

    vp9_variance8x4 = vp9_variance8x4_c;
    //if (flags & HAS_SSE2) vp9_variance8x4 = vp9_variance8x4_sse2;

    vp9_variance4x8 = vp9_variance4x8_c;
    //if (flags & HAS_SSE2) vp9_variance4x8 = vp9_variance4x8_sse2;

    vp9_variance4x4 = vp9_variance4x4_c;
    //if (flags & HAS_MMX) vp9_variance4x4 = vp9_variance4x4_mmx;
    //if (flags & HAS_SSE2) vp9_variance4x4 = vp9_variance4x4_sse2;

    vp9_sub_pixel_variance64x64 = vp9_sub_pixel_variance64x64_c;
    //if (flags & HAS_SSE2) vp9_sub_pixel_variance64x64 = vp9_sub_pixel_variance64x64_sse2;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_variance64x64 = vp9_sub_pixel_variance64x64_ssse3;

    vp9_sub_pixel_avg_variance64x64 = vp9_sub_pixel_avg_variance64x64_c;
    //if (flags & HAS_SSE2) vp9_sub_pixel_avg_variance64x64 = vp9_sub_pixel_avg_variance64x64_sse2;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_avg_variance64x64 = vp9_sub_pixel_avg_variance64x64_ssse3;

    vp9_sub_pixel_variance32x64 = vp9_sub_pixel_variance32x64_c;
    //if (flags & HAS_SSE2) vp9_sub_pixel_variance32x64 = vp9_sub_pixel_variance32x64_sse2;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_variance32x64 = vp9_sub_pixel_variance32x64_ssse3;

    vp9_sub_pixel_avg_variance32x64 = vp9_sub_pixel_avg_variance32x64_c;
    //if (flags & HAS_SSE2) vp9_sub_pixel_avg_variance32x64 = vp9_sub_pixel_avg_variance32x64_sse2;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_avg_variance32x64 = vp9_sub_pixel_avg_variance32x64_ssse3;

    vp9_sub_pixel_variance64x32 = vp9_sub_pixel_variance64x32_c;
    //if (flags & HAS_SSE2) vp9_sub_pixel_variance64x32 = vp9_sub_pixel_variance64x32_sse2;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_variance64x32 = vp9_sub_pixel_variance64x32_ssse3;

    vp9_sub_pixel_avg_variance64x32 = vp9_sub_pixel_avg_variance64x32_c;
    //if (flags & HAS_SSE2) vp9_sub_pixel_avg_variance64x32 = vp9_sub_pixel_avg_variance64x32_sse2;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_avg_variance64x32 = vp9_sub_pixel_avg_variance64x32_ssse3;

    vp9_sub_pixel_variance32x16 = vp9_sub_pixel_variance32x16_c;
    //if (flags & HAS_SSE2) vp9_sub_pixel_variance32x16 = vp9_sub_pixel_variance32x16_sse2;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_variance32x16 = vp9_sub_pixel_variance32x16_ssse3;

    vp9_sub_pixel_avg_variance32x16 = vp9_sub_pixel_avg_variance32x16_c;
    //if (flags & HAS_SSE2) vp9_sub_pixel_avg_variance32x16 = vp9_sub_pixel_avg_variance32x16_sse2;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_avg_variance32x16 = vp9_sub_pixel_avg_variance32x16_ssse3;

    vp9_sub_pixel_variance16x32 = vp9_sub_pixel_variance16x32_c;
    //if (flags & HAS_SSE2) vp9_sub_pixel_variance16x32 = vp9_sub_pixel_variance16x32_sse2;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_variance16x32 = vp9_sub_pixel_variance16x32_ssse3;

    vp9_sub_pixel_avg_variance16x32 = vp9_sub_pixel_avg_variance16x32_c;
    //if (flags & HAS_SSE2) vp9_sub_pixel_avg_variance16x32 = vp9_sub_pixel_avg_variance16x32_sse2;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_avg_variance16x32 = vp9_sub_pixel_avg_variance16x32_ssse3;

    vp9_sub_pixel_variance32x32 = vp9_sub_pixel_variance32x32_c;
    //if (flags & HAS_SSE2) vp9_sub_pixel_variance32x32 = vp9_sub_pixel_variance32x32_sse2;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_variance32x32 = vp9_sub_pixel_variance32x32_ssse3;

    vp9_sub_pixel_avg_variance32x32 = vp9_sub_pixel_avg_variance32x32_c;
    //if (flags & HAS_SSE2) vp9_sub_pixel_avg_variance32x32 = vp9_sub_pixel_avg_variance32x32_sse2;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_avg_variance32x32 = vp9_sub_pixel_avg_variance32x32_ssse3;

    vp9_sub_pixel_variance16x16 = vp9_sub_pixel_variance16x16_c;
    //if (flags & HAS_SSE2) vp9_sub_pixel_variance16x16 = vp9_sub_pixel_variance16x16_sse2;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_variance16x16 = vp9_sub_pixel_variance16x16_ssse3;

    vp9_sub_pixel_avg_variance16x16 = vp9_sub_pixel_avg_variance16x16_c;
    //if (flags & HAS_SSE2) vp9_sub_pixel_avg_variance16x16 = vp9_sub_pixel_avg_variance16x16_sse2;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_avg_variance16x16 = vp9_sub_pixel_avg_variance16x16_ssse3;

    vp9_sub_pixel_variance8x16 = vp9_sub_pixel_variance8x16_c;
    //if (flags & HAS_SSE2) vp9_sub_pixel_variance8x16 = vp9_sub_pixel_variance8x16_sse2;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_variance8x16 = vp9_sub_pixel_variance8x16_ssse3;

    vp9_sub_pixel_avg_variance8x16 = vp9_sub_pixel_avg_variance8x16_c;
    //if (flags & HAS_SSE2) vp9_sub_pixel_avg_variance8x16 = vp9_sub_pixel_avg_variance8x16_sse2;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_avg_variance8x16 = vp9_sub_pixel_avg_variance8x16_ssse3;

    vp9_sub_pixel_variance16x8 = vp9_sub_pixel_variance16x8_c;
    //if (flags & HAS_SSE2) vp9_sub_pixel_variance16x8 = vp9_sub_pixel_variance16x8_sse2;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_variance16x8 = vp9_sub_pixel_variance16x8_ssse3;

    vp9_sub_pixel_avg_variance16x8 = vp9_sub_pixel_avg_variance16x8_c;
    //if (flags & HAS_SSE2) vp9_sub_pixel_avg_variance16x8 = vp9_sub_pixel_avg_variance16x8_sse2;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_avg_variance16x8 = vp9_sub_pixel_avg_variance16x8_ssse3;

    vp9_sub_pixel_variance8x8 = vp9_sub_pixel_variance8x8_c;
    //if (flags & HAS_SSE2) vp9_sub_pixel_variance8x8 = vp9_sub_pixel_variance8x8_sse2;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_variance8x8 = vp9_sub_pixel_variance8x8_ssse3;

    vp9_sub_pixel_avg_variance8x8 = vp9_sub_pixel_avg_variance8x8_c;
    //if (flags & HAS_SSE2) vp9_sub_pixel_avg_variance8x8 = vp9_sub_pixel_avg_variance8x8_sse2;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_avg_variance8x8 = vp9_sub_pixel_avg_variance8x8_ssse3;

    vp9_sub_pixel_variance8x4 = vp9_sub_pixel_variance8x4_c;
    //if (flags & HAS_SSE2) vp9_sub_pixel_variance8x4 = vp9_sub_pixel_variance8x4_sse2;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_variance8x4 = vp9_sub_pixel_variance8x4_ssse3;

    vp9_sub_pixel_avg_variance8x4 = vp9_sub_pixel_avg_variance8x4_c;
    //if (flags & HAS_SSE2) vp9_sub_pixel_avg_variance8x4 = vp9_sub_pixel_avg_variance8x4_sse2;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_avg_variance8x4 = vp9_sub_pixel_avg_variance8x4_ssse3;

    vp9_sub_pixel_variance4x8 = vp9_sub_pixel_variance4x8_c;
    //if (flags & HAS_SSE) vp9_sub_pixel_variance4x8 = vp9_sub_pixel_variance4x8_sse;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_variance4x8 = vp9_sub_pixel_variance4x8_ssse3;

    vp9_sub_pixel_avg_variance4x8 = vp9_sub_pixel_avg_variance4x8_c;
    //if (flags & HAS_SSE) vp9_sub_pixel_avg_variance4x8 = vp9_sub_pixel_avg_variance4x8_sse;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_avg_variance4x8 = vp9_sub_pixel_avg_variance4x8_ssse3;

    vp9_sub_pixel_variance4x4 = vp9_sub_pixel_variance4x4_c;
    //if (flags & HAS_SSE) vp9_sub_pixel_variance4x4 = vp9_sub_pixel_variance4x4_sse;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_variance4x4 = vp9_sub_pixel_variance4x4_ssse3;

    vp9_sub_pixel_avg_variance4x4 = vp9_sub_pixel_avg_variance4x4_c;
    //if (flags & HAS_SSE) vp9_sub_pixel_avg_variance4x4 = vp9_sub_pixel_avg_variance4x4_sse;
    //if (flags & HAS_SSSE3) vp9_sub_pixel_avg_variance4x4 = vp9_sub_pixel_avg_variance4x4_ssse3;

    vp9_sad64x64 = vp9_sad64x64_c;
    //if (flags & HAS_SSE2) vp9_sad64x64 = vp9_sad64x64_sse2;

    vp9_sad32x64 = vp9_sad32x64_c;
    //if (flags & HAS_SSE2) vp9_sad32x64 = vp9_sad32x64_sse2;

    vp9_sad64x32 = vp9_sad64x32_c;
    //if (flags & HAS_SSE2) vp9_sad64x32 = vp9_sad64x32_sse2;

    vp9_sad32x16 = vp9_sad32x16_c;
    //if (flags & HAS_SSE2) vp9_sad32x16 = vp9_sad32x16_sse2;

    vp9_sad16x32 = vp9_sad16x32_c;
    //if (flags & HAS_SSE2) vp9_sad16x32 = vp9_sad16x32_sse2;

    vp9_sad32x32 = vp9_sad32x32_c;
    //if (flags & HAS_SSE2) vp9_sad32x32 = vp9_sad32x32_sse2;

    vp9_sad16x16 = vp9_sad16x16_c;
    //if (flags & HAS_MMX) vp9_sad16x16 = vp9_sad16x16_mmx;
    //if (flags & HAS_SSE2) vp9_sad16x16 = vp9_sad16x16_sse2;

    vp9_sad16x8 = vp9_sad16x8_c;
    //if (flags & HAS_MMX) vp9_sad16x8 = vp9_sad16x8_mmx;
    //if (flags & HAS_SSE2) vp9_sad16x8 = vp9_sad16x8_sse2;

    vp9_sad8x16 = vp9_sad8x16_c;
    //if (flags & HAS_MMX) vp9_sad8x16 = vp9_sad8x16_mmx;
    //if (flags & HAS_SSE2) vp9_sad8x16 = vp9_sad8x16_sse2;

    vp9_sad8x8 = vp9_sad8x8_c;
    //if (flags & HAS_MMX) vp9_sad8x8 = vp9_sad8x8_mmx;
    //if (flags & HAS_SSE2) vp9_sad8x8 = vp9_sad8x8_sse2;

    vp9_sad8x4 = vp9_sad8x4_c;
    //if (flags & HAS_SSE2) vp9_sad8x4 = vp9_sad8x4_sse2;

    vp9_sad4x8 = vp9_sad4x8_c;
    //if (flags & HAS_SSE) vp9_sad4x8 = vp9_sad4x8_sse;

    vp9_sad4x4 = vp9_sad4x4_c;
    //if (flags & HAS_MMX) vp9_sad4x4 = vp9_sad4x4_mmx;
    //if (flags & HAS_SSE) vp9_sad4x4 = vp9_sad4x4_sse;

    vp9_sad64x64_avg = vp9_sad64x64_avg_c;
    //if (flags & HAS_SSE2) vp9_sad64x64_avg = vp9_sad64x64_avg_sse2;

    vp9_sad32x64_avg = vp9_sad32x64_avg_c;
    //if (flags & HAS_SSE2) vp9_sad32x64_avg = vp9_sad32x64_avg_sse2;

    vp9_sad64x32_avg = vp9_sad64x32_avg_c;
    //if (flags & HAS_SSE2) vp9_sad64x32_avg = vp9_sad64x32_avg_sse2;

    vp9_sad32x16_avg = vp9_sad32x16_avg_c;
    //if (flags & HAS_SSE2) vp9_sad32x16_avg = vp9_sad32x16_avg_sse2;

    vp9_sad16x32_avg = vp9_sad16x32_avg_c;
    //if (flags & HAS_SSE2) vp9_sad16x32_avg = vp9_sad16x32_avg_sse2;

    vp9_sad32x32_avg = vp9_sad32x32_avg_c;
    //if (flags & HAS_SSE2) vp9_sad32x32_avg = vp9_sad32x32_avg_sse2;

    vp9_sad16x16_avg = vp9_sad16x16_avg_c;
    //if (flags & HAS_SSE2) vp9_sad16x16_avg = vp9_sad16x16_avg_sse2;

    vp9_sad16x8_avg = vp9_sad16x8_avg_c;
    //if (flags & HAS_SSE2) vp9_sad16x8_avg = vp9_sad16x8_avg_sse2;

    vp9_sad8x16_avg = vp9_sad8x16_avg_c;
    //if (flags & HAS_SSE2) vp9_sad8x16_avg = vp9_sad8x16_avg_sse2;

    vp9_sad8x8_avg = vp9_sad8x8_avg_c;
    //if (flags & HAS_SSE2) vp9_sad8x8_avg = vp9_sad8x8_avg_sse2;

    vp9_sad8x4_avg = vp9_sad8x4_avg_c;
    //if (flags & HAS_SSE2) vp9_sad8x4_avg = vp9_sad8x4_avg_sse2;

    vp9_sad4x8_avg = vp9_sad4x8_avg_c;
    //if (flags & HAS_SSE) vp9_sad4x8_avg = vp9_sad4x8_avg_sse;

    vp9_sad4x4_avg = vp9_sad4x4_avg_c;
    //if (flags & HAS_SSE) vp9_sad4x4_avg = vp9_sad4x4_avg_sse;

    vp9_variance_halfpixvar16x16_h = vp9_variance_halfpixvar16x16_h_c;
    //if (flags & HAS_SSE2) vp9_variance_halfpixvar16x16_h = vp9_variance_halfpixvar16x16_h_sse2;

    vp9_variance_halfpixvar16x16_v = vp9_variance_halfpixvar16x16_v_c;
    //if (flags & HAS_SSE2) vp9_variance_halfpixvar16x16_v = vp9_variance_halfpixvar16x16_v_sse2;

    vp9_variance_halfpixvar16x16_hv = vp9_variance_halfpixvar16x16_hv_c;
    //if (flags & HAS_SSE2) vp9_variance_halfpixvar16x16_hv = vp9_variance_halfpixvar16x16_hv_sse2;









    vp9_sad16x16x3 = vp9_sad16x16x3_c;
    //if (flags & HAS_SSE3) vp9_sad16x16x3 = vp9_sad16x16x3_sse3;
    //if (flags & HAS_SSSE3) vp9_sad16x16x3 = vp9_sad16x16x3_ssse3;

    vp9_sad16x8x3 = vp9_sad16x8x3_c;
    //if (flags & HAS_SSE3) vp9_sad16x8x3 = vp9_sad16x8x3_sse3;
    //if (flags & HAS_SSSE3) vp9_sad16x8x3 = vp9_sad16x8x3_ssse3;

    vp9_sad8x16x3 = vp9_sad8x16x3_c;
    //if (flags & HAS_SSE3) vp9_sad8x16x3 = vp9_sad8x16x3_sse3;

    vp9_sad8x8x3 = vp9_sad8x8x3_c;
    //if (flags & HAS_SSE3) vp9_sad8x8x3 = vp9_sad8x8x3_sse3;

    vp9_sad4x4x3 = vp9_sad4x4x3_c;
    //if (flags & HAS_SSE3) vp9_sad4x4x3 = vp9_sad4x4x3_sse3;










    vp9_sad64x64x4d = vp9_sad64x64x4d_c;
    //if (flags & HAS_SSE2) vp9_sad64x64x4d = vp9_sad64x64x4d_sse2;

    vp9_sad32x64x4d = vp9_sad32x64x4d_c;
    //if (flags & HAS_SSE2) vp9_sad32x64x4d = vp9_sad32x64x4d_sse2;

    vp9_sad64x32x4d = vp9_sad64x32x4d_c;
    //if (flags & HAS_SSE2) vp9_sad64x32x4d = vp9_sad64x32x4d_sse2;

    vp9_sad32x16x4d = vp9_sad32x16x4d_c;
    //if (flags & HAS_SSE2) vp9_sad32x16x4d = vp9_sad32x16x4d_sse2;

    vp9_sad16x32x4d = vp9_sad16x32x4d_c;
    //if (flags & HAS_SSE2) vp9_sad16x32x4d = vp9_sad16x32x4d_sse2;

    vp9_sad32x32x4d = vp9_sad32x32x4d_c;
    //if (flags & HAS_SSE2) vp9_sad32x32x4d = vp9_sad32x32x4d_sse2;

    vp9_sad16x16x4d = vp9_sad16x16x4d_c;
    //if (flags & HAS_SSE2) vp9_sad16x16x4d = vp9_sad16x16x4d_sse2;

    vp9_sad16x8x4d = vp9_sad16x8x4d_c;
    //if (flags & HAS_SSE2) vp9_sad16x8x4d = vp9_sad16x8x4d_sse2;

    vp9_sad8x16x4d = vp9_sad8x16x4d_c;
    //if (flags & HAS_SSE2) vp9_sad8x16x4d = vp9_sad8x16x4d_sse2;

    vp9_sad8x8x4d = vp9_sad8x8x4d_c;
    //if (flags & HAS_SSE2) vp9_sad8x8x4d = vp9_sad8x8x4d_sse2;

    vp9_sad8x4x4d = vp9_sad8x4x4d_c;
    //if (flags & HAS_SSE2) vp9_sad8x4x4d = vp9_sad8x4x4d_sse2;

    vp9_sad4x8x4d = vp9_sad4x8x4d_c;
    //if (flags & HAS_SSE) vp9_sad4x8x4d = vp9_sad4x8x4d_sse;

    vp9_sad4x4x4d = vp9_sad4x4x4d_c;
    //if (flags & HAS_SSE) vp9_sad4x4x4d = vp9_sad4x4x4d_sse;

    vp9_mse16x16 = vp9_mse16x16_c;
    //if (flags & HAS_MMX) vp9_mse16x16 = vp9_mse16x16_mmx;
    //if (flags & HAS_SSE2) vp9_mse16x16 = vp9_mse16x16_sse2;






    vp9_get_mb_ss = vp9_get_mb_ss_c;
    //if (flags & HAS_MMX) vp9_get_mb_ss = vp9_get_mb_ss_mmx;
    //if (flags & HAS_SSE2) vp9_get_mb_ss = vp9_get_mb_ss_sse2;

    vp9_block_error = vp9_block_error_c;
    //if (flags & HAS_SSE2) vp9_block_error = vp9_block_error_sse2;

    vp9_subtract_block = vp9_subtract_block_c;
    //if (flags & HAS_SSE2) vp9_subtract_block = vp9_subtract_block_sse2;



    vp9_short_fht4x4 = vp9_short_fht4x4_c;
    //if (flags & HAS_SSE2) vp9_short_fht4x4 = vp9_short_fht4x4_sse2;

    vp9_short_fht8x8 = vp9_short_fht8x8_c;
    //if (flags & HAS_SSE2) vp9_short_fht8x8 = vp9_short_fht8x8_sse2;

    vp9_short_fht16x16 = vp9_short_fht16x16_c;
    //if (flags & HAS_SSE2) vp9_short_fht16x16 = vp9_short_fht16x16_sse2;

    vp9_short_fdct8x8 = vp9_short_fdct8x8_c;
    //if (flags & HAS_SSE2) vp9_short_fdct8x8 = vp9_short_fdct8x8_sse2;

    vp9_short_fdct4x4 = vp9_short_fdct4x4_c;
    //if (flags & HAS_SSE2) vp9_short_fdct4x4 = vp9_short_fdct4x4_sse2;

    vp9_short_fdct8x4 = vp9_short_fdct8x4_c;
    //if (flags & HAS_SSE2) vp9_short_fdct8x4 = vp9_short_fdct8x4_sse2;

    vp9_short_fdct32x32 = vp9_short_fdct32x32_c;
    //if (flags & HAS_SSE2) vp9_short_fdct32x32 = vp9_short_fdct32x32_sse2;

    vp9_short_fdct32x32_rd = vp9_short_fdct32x32_rd_c;
    //if (flags & HAS_SSE2) vp9_short_fdct32x32_rd = vp9_short_fdct32x32_rd_sse2;

    vp9_short_fdct16x16 = vp9_short_fdct16x16_c;
    //if (flags & HAS_SSE2) vp9_short_fdct16x16 = vp9_short_fdct16x16_sse2;



    vp9_full_search_sad = vp9_full_search_sad_c;
    //if (flags & HAS_SSE3) vp9_full_search_sad = vp9_full_search_sadx3;
    //if (flags & HAS_SSE4_1) vp9_full_search_sad = vp9_full_search_sadx8;

    vp9_refining_search_sad = vp9_refining_search_sad_c;
    //if (flags & HAS_SSE3) vp9_refining_search_sad = vp9_refining_search_sadx4;

    vp9_diamond_search_sad = vp9_diamond_search_sad_c;
    //if (flags & HAS_SSE3) vp9_diamond_search_sad = vp9_diamond_search_sadx4;

    vp9_temporal_filter_apply = vp9_temporal_filter_apply_c;
    //if (flags & HAS_SSE2) vp9_temporal_filter_apply = vp9_temporal_filter_apply_sse2;
}
#endif
#endif
