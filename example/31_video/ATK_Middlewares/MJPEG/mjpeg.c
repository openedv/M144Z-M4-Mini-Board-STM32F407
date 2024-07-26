/**
 ****************************************************************************************************
 * @file        mjpeg.c
 * @author      ALIENTEK
 * @brief       MJPEG video processing code
 * @license     Copyright (C) 2012-2024, ALIENTEK
 ****************************************************************************************************
 * @attention
 *
 * platform     : ALIENTEK STM32F407 development board
 * website      : www.alientek.com
 * forum        : www.openedv.com/forum.php
 *
 * change logs  ：
 * version      data         notes
 * V1.0         20240409     the first version
 *
 ****************************************************************************************************
 */

#include "../../ATK_Middlewares/MJPEG/mjpeg.h"
#include "../../ATK_Middlewares/MALLOC/malloc.h"
#include "../../BSP/LCD/lcd.h"
#include "ff.h"

/* �򵥿��ٵ��ڴ����,������ٶ� */
#define MJPEG_MAX_MALLOC_SIZE       38 * 1024     /* �����Է���38K�ֽ� */

struct jpeg_decompress_struct *cinfo;
struct my_error_mgr *jerr;
uint8_t *jpegbuf;           /* jpeg���ݻ���ָ�� */
uint32_t jbufsize;          /* jpeg buf��С */
uint16_t imgoffx, imgoffy;  /* ͼ����x,y�����ƫ���� */
uint8_t *jmembuf;           /* mjpeg����� �ڴ�� */
uint32_t jmempos;           /* �ڴ��ָ�� */

/**
 * @brief       mjpeg�����ڴ�
 * @param       num   : ��һ�η���ĵ�ַ��ʼ��ַ
 * @retval      ��
 */
void *mjpeg_malloc(uint32_t num)
{
    uint32_t curpos = jmempos;  /* �˴η������ʼ��ַ */
    jmempos += num;             /* ��һ�η������ʼ��ַ */

    if (jmempos > 38 * 1024)
    {
        printf("mem error:%d,%d", curpos, num);
    }

    return (void *)&jmembuf[curpos];    /* �������뵽���ڴ��׵�ַ */
}

/**
 * @brief       �����˳�
 * @param       cinfo   : JPEG���������ƽṹ��
 * @retval      ��
 */
static void my_error_exit(j_common_ptr cinfo)
{
    my_error_ptr myerr = (my_error_ptr) cinfo->err;
    (*cinfo->err->output_message) (cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}

/**
 * @brief       ������Ϣ
 * @param       cinfo       : JPEG���������ƽṹ��
 * @param       msg_level   : ��Ϣ�ȼ�
 * @retval      ��
 */
METHODDEF(void) my_emit_message(j_common_ptr cinfo, int msg_level)
{
    my_error_ptr myerr = (my_error_ptr) cinfo->err;

    if (msg_level < 0)
    {
        printf("emit msg:%d\r\n", msg_level);
        longjmp(myerr->setjmp_buffer, 1);
    }
}

/**
 * @brief       ��ʼ����Դ,��ִ���κβ���
 * @param       cinfo   : JPEG���������ƽṹ��
 * @retval      ��
 */
static void init_source(j_decompress_ptr cinfo)
{
    /* ����Ҫ���κ�����. */
    return;
}

/**
 * @brief       ������뻺����,һ���Զ�ȡ��֡����
 * @param       cinfo   : JPEG���������ƽṹ��
 * @retval      ��
 */
static boolean fill_input_buffer(j_decompress_ptr cinfo)
{
    if (jbufsize == 0) /* ������ */
    {
        printf("jd read off\r\n");
        jpegbuf[0] = (uint8_t) 0xFF;        /* �������� */
        jpegbuf[1] = (uint8_t) JPEG_EOI;
        cinfo->src->next_input_byte = jpegbuf;
        cinfo->src->bytes_in_buffer = 2;
    }
    else
    {
        cinfo->src->next_input_byte = jpegbuf;
        cinfo->src->bytes_in_buffer = jbufsize;
        jbufsize -= jbufsize;
    }

    return TRUE;
}

/**
 * @brief       ���ļ�����,����num_bytes������
 * @param       cinfo       : JPEG���������ƽṹ��
 * @param       num_bytes   : �����ֽڳ���
 * @retval      ��
 */
static void skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
    /* Just a dumb implementation for now.  Could use fseek() except
    * it doesn't work on pipes.  Not clear that being smart is worth
    * any trouble anyway --- large skips are infrequent.
    */
    if (num_bytes > 0)
    {
        while (num_bytes > (long) cinfo->src->bytes_in_buffer)
        {
            num_bytes -= (long)cinfo->src->bytes_in_buffer;
            (void)cinfo->src->fill_input_buffer(cinfo);
            /* note we assume that fill_input_buffer will never
            * return FALSE, so suspension need not be handled.
            */
        }

        cinfo->src->next_input_byte += (size_t) num_bytes;
        cinfo->src->bytes_in_buffer -= (size_t) num_bytes;
    }
}

/**
 * @brief       �ڽ��������,��jpeg_finish_decompress��������
 * @param       cinfo       : JPEG���������ƽṹ��
 * @retval      ��
 */
static void term_source(j_decompress_ptr cinfo)
{
    /* �����κδ��� */
    return;
}

/**
 * @brief       ��ʼ��jpeg��������Դ
 * @param       cinfo   : JPEG���������ƽṹ��
 * @retval      ��
 */
static void jpeg_filerw_src_init(j_decompress_ptr cinfo)
{
    if (cinfo->src == NULL)     /* first time for this JPEG object? */
    {
        cinfo->src = (struct jpeg_source_mgr *)
                     (*cinfo->mem->alloc_small)((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                                sizeof(struct jpeg_source_mgr));
    }

    cinfo->src->init_source = init_source;
    cinfo->src->fill_input_buffer = fill_input_buffer;
    cinfo->src->skip_input_data = skip_input_data;
    cinfo->src->resync_to_restart = jpeg_resync_to_restart; /* use default method */
    cinfo->src->term_source = term_source;
    cinfo->src->bytes_in_buffer = 0;    /* forces fill_input_buffer on first read */
    cinfo->src->next_input_byte = NULL; /* until buffer loaded */
}

/**
 * @brief       ��ʼ��jpeg��������Դ
 * @param       offx   : x�����ƫ��
 * @param       offy   : y�����ƫ��
 * @retval      0,�ɹ�;
 *              1,ʧ��
 */
uint8_t mjpegdec_init(uint16_t offx, uint16_t offy)
{
    cinfo = mymalloc(SRAMCCM, sizeof(struct jpeg_decompress_struct));
    jerr = mymalloc(SRAMCCM, sizeof(struct my_error_mgr));
    jmembuf = mymalloc(SRAMCCM, MJPEG_MAX_MALLOC_SIZE);     /* MJPEG�����ڴ������ */

    if (cinfo == 0 || jerr == 0 || jmembuf == 0)
    {
        mjpegdec_free();
        return 1;
    }

    /* ����ͼ����x,y�����ƫ���� */
    imgoffx = offx;
    imgoffy = offy;
    
    return 0;
}

/**
 * @brief       mjpeg����,�ͷ��ڴ�
 * @param       ��
 * @retval      ��
 */
void mjpegdec_free(void)
{
    myfree(SRAMCCM, cinfo);
    myfree(SRAMCCM, jerr);
    myfree(SRAMCCM, jmembuf);
}

/**
 * @brief       ����һ��JPEGͼƬ
 * @param       buf     : jpeg����������
 * @param       bsize   : �����С
 * @retval      0,�ɹ�;
 *              ����,����
 */
uint8_t mjpegdec_decode(uint8_t *buf, uint32_t bsize)
{
    JSAMPARRAY buffer;

    if (bsize == 0)return 1;

    jpegbuf = buf;
    jbufsize = bsize;
    jmempos = 0; /* MJEPG����,���´�0��ʼ�����ڴ� */

    cinfo->err = jpeg_std_error(&jerr->pub);
    jerr->pub.error_exit = my_error_exit;
    jerr->pub.emit_message = my_emit_message;

    //if(bsize>20*1024)printf("s:%d\r\n",bsize);
    if (setjmp(jerr->setjmp_buffer))    /* ������ */
    {
        jpeg_abort_decompress(cinfo);
        jpeg_destroy_decompress(cinfo);
        return 2;
    }

    jpeg_create_decompress(cinfo);
    jpeg_filerw_src_init(cinfo);
    jpeg_read_header(cinfo, TRUE);
    cinfo->dct_method = JDCT_IFAST;
    cinfo->do_fancy_upsampling = 0;
    jpeg_start_decompress(cinfo);
    lcd_set_window(imgoffx, imgoffy, cinfo->output_width, cinfo->output_height);
    lcd_write_ram_prepare();        /* ��ʼд��GRAM */

    while (cinfo->output_scanline < cinfo->output_height)
    {
        jpeg_read_scanlines(cinfo, buffer, 1);
    }

    lcd_set_window(0, 0, lcddev.width, lcddev.height);  /* �ָ����� */
    jpeg_finish_decompress(cinfo);
    jpeg_destroy_decompress(cinfo);
    
    return 0;
}
