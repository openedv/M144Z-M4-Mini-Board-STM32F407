/**
 ****************************************************************************************************
 * @file        videoplayer.c
 * @author      ALIENTEK
 * @brief       Video player application code
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

#include "../../SYSTEM/delay/delay.h"
#include "usart.h"
#include "../../BSP/KEY/key.h"
#include "../../BSP/LCD/lcd.h"
#include "tim.h"
#include "videoplayer.h"
#include "../../FatFs/exfuns/exfuns.h"
#include "../../ATK_Middlewares/MALLOC/malloc.h"
#include "../../ATK_Middlewares/MJPEG/avi.h"
#include "../../ATK_Middlewares/MJPEG/mjpeg.h"
#include <string.h>
#include <stdio.h>

uint16_t frame;
__IO uint8_t frameup;

/**
 * @brief       ��ȡָ��·������Ч��Ƶ�ļ�������
 * @param       path: ָ��·��
 * @retval      ��Ч��Ƶ�ļ�������
 */
static uint16_t video_get_tnum(char *path)
{
    uint8_t res;
    uint16_t rval = 0;
    DIR tdir;
    FILINFO *tfileinfo;
    
    tfileinfo = (FILINFO *)mymalloc(SRAMIN, sizeof(FILINFO));   /* �����ڴ� */
    res = (uint8_t)f_opendir(&tdir, (const TCHAR *)path);       /* ��Ŀ¼ */
    if ((res == 0) && tfileinfo)
    {
        while (1)                                               /* ��ѯ�ܵ���Ч�ļ��� */
        {
            res = (uint8_t)f_readdir(&tdir, tfileinfo);         /* ��ȡĿ¼�µ�һ���ļ� */
            if ((res != 0) || (tfileinfo->fname[0] == 0))       /* �����ĩβ���˳� */
            {
                break;
            }
            
            res = exfuns_file_type(tfileinfo->fname);
            if ((res & 0xF0) == 0x60)                           /* ����Ƶ�ļ� */
            {
                rval++;
            }
        }
    }
    
    myfree(SRAMIN, tfileinfo);                                  /* �ͷ��ڴ� */
    
    return rval;
}

/**
 * @brief       ��ʾ��Ƶ������Ϣ
 * @param       name : ��Ƶ����
 * @param       index: ��ǰ����
 * @param       total: ���ļ���
 * @retval      ��
 */
static void video_bmsg_show(uint8_t *name, uint16_t index, uint16_t total)
{
    uint8_t *buf;
    
    buf = mymalloc(SRAMIN, 100);
    
    sprintf((char *)buf, "Filename:%s", name);
    //text_show_string(10, 10, lcddev.width - 10, 16, (char *)buf, 16, 0, RED);   /* ��ʾ�ļ��� */
    lcd_show_string(10, 10, lcddev.width - 10, 16, 16, (char *)buf, RED);
    sprintf((char *)buf, "Indexes:%d/%d", index, total);
    //text_show_string(10, 30, lcddev.width - 10, 16, (char *)buf, 16, 0, RED);   /* ��ʾ���� */
    lcd_show_string(10, 30, lcddev.width - 10, 16, 16, (char *)buf, RED);

    myfree(SRAMIN, buf);
}

/**
 * @brief       ��ʾ��ǰ��Ƶ�ļ��������Ϣ
 * @param       aviinfo: avi���ƽṹ��
 * @retval      ��
 */
static void video_info_show(AVI_INFO *aviinfo)
{
    uint8_t *buf;
    
    buf = mymalloc(SRAMIN, 100);
    
    sprintf((char *)buf, "Sound channel:%d,Sampling rate:%d", aviinfo->Channels, aviinfo->SampleRate * 10);
    //text_show_string(10, 50, lcddev.width - 10, 16, (char *)buf, 16, 0, RED);   /* ��ʾ�������� */
    lcd_show_string(10, 50, lcddev.width - 10, 16, 16, (char *)buf, RED);

    sprintf((char *)buf, "Frame rate:%d֡", 1000 / (aviinfo->SecPerFrame / 1000));
    //text_show_string(10, 70, lcddev.width - 10, 16, (char *)buf, 16, 0, RED);   /* ��ʾ�������� */
    lcd_show_string(10, 70, lcddev.width - 10, 16, 16, (char *)buf, RED);

    myfree(SRAMIN, buf);
}

/**
 * @brief       ��ʾ��ǰ����ʱ��
 * @param       favi   : ��ǰ���ŵ���Ƶ�ļ�
 * @param       aviinfo: avi���ƽṹ��
 * @retval      ��
 */
void video_time_show(FIL *favi, AVI_INFO *aviinfo)
{
    static uint32_t oldsec; /* ��һ�εĲ���ʱ�� */
    uint8_t *buf;
    uint32_t totsec = 0;    /* video�ļ���ʱ�� */
    uint32_t cursec;        /* ��ǰ����ʱ�� */
    
    totsec = (aviinfo->SecPerFrame / 1000) * aviinfo->TotalFrame;                   /* �����ܳ���(��λ:ms) */
    totsec /= 1000;                                                                 /* ������. */
    cursec = ((double)favi->fptr / favi->obj.objsize) * totsec;                     /* ��ȡ��ǰ���ŵ������� */
    
    if (oldsec != cursec)                                                           /* ��Ҫ������ʾʱ�� */
    {
        buf = mymalloc(SRAMIN, 100);
        oldsec = cursec;
        
        sprintf((char *)buf, "Play time:%02d:%02d:%02d/%02d:%02d:%02d", cursec / 3600, (cursec % 3600) / 60, cursec % 60, totsec / 3600, (totsec % 3600) / 60, totsec % 60);
        //text_show_string(10, 90, lcddev.width - 10, 16, (char *)buf, 16, 0, RED);   /* ��ʾ�������� */
        lcd_show_string(10, 90, lcddev.width - 10, 16, 16, (char *)buf, RED);

        myfree(SRAMIN, buf);
    }
}

/**
 * @brief       ����MJPEG��Ƶ
 * @param       pname: ��Ƶ�ļ���
 * @retval      ������ֵ
 *              KEY0_PRES: ��һ����Ƶ
 *              WKUP_PRES: ��һ����Ƶ
 *              ����ֵ   : �������
 */
static uint8_t video_play_mjpeg(uint8_t *pname)
{
    uint8_t *framebuf;
    uint8_t *pbuf;
    uint8_t res = 0;
    uint16_t offset;
    uint32_t nr;
    uint8_t key;
    FIL *favi;
    uint8_t *psaibuf;
    
    framebuf = (uint8_t *)mymalloc(SRAMIN, AVI_VIDEO_BUF_SIZE); /* �����ڴ� */
    favi = (FIL *)mymalloc(SRAMIN, sizeof(FIL));
    psaibuf = (uint8_t *)mymalloc(SRAMIN, AVI_AUDIO_BUF_SIZE);
    if ((framebuf == NULL) || (favi == NULL) || (psaibuf == NULL))
    {
        printf("memory error!\r\n");
        res = 0xFF;
    }
    memset(psaibuf, 0, AVI_AUDIO_BUF_SIZE);
    
    while (res == 0)
    {
        res = (uint8_t)f_open(favi, (const TCHAR *)pname, FA_READ);                                                     /* ���ļ� */
        if (res == 0)
        {
            pbuf = framebuf;
            res = (uint8_t)f_read(favi, pbuf, AVI_VIDEO_BUF_SIZE, &nr);                                                 /* ��ʼ��ȡ */
            if (res != 0)
            {
                printf("fread error:%d\r\n", res);
                break;
            }
            
            res = avi_init(pbuf, AVI_VIDEO_BUF_SIZE);                                                                   /* AVI���� */
            if (res != 0)
            {
                printf("avi error:%d\r\n", res);
                break;
            }
            
            video_info_show(&g_avix);
            MX_TIM7_Init();
            //btim_tim7_int_init(g_avix.SecPerFrame / 100 - 1, 8400 - 1);                                                 /* ��ʼ��������ʱ��7�жϣ�������֡������Ƶ */
            offset = avi_srarch_id(pbuf, AVI_VIDEO_BUF_SIZE, "movi");                                                   /* Ѱ��movi ID */
            avi_get_streaminfo(pbuf + offset + 4);                                                                      /* ��ȡ����Ϣ */
            f_lseek(favi, offset + 12);                                                                                 /* ������־ID������ַƫ�Ƶ������ݿ�ʼ�� */
            res = mjpegdec_init((lcddev.width - g_avix.Width) / 2, 110 + (lcddev.height - 110 - g_avix.Height) / 2);    /* ��ʼ��JPG���� */
            
            while (1)
            {
                if (g_avix.StreamID == AVI_VIDS_FLAG)                                                                   /* ��Ƶ�� */
                {
                    pbuf = framebuf;
                    f_read(favi, pbuf, g_avix.StreamSize + 8, &nr);                                                     /* ��ȡ��֡+��һ֡������ID��Ϣ */
                    res = mjpegdec_decode(pbuf, g_avix.StreamSize);
                    if (res != 0)
                    {
                        printf("decode error!\r\n");
                    }
                    
                    while (frameup == 0);                                                                               /* �ȴ�����ʱ�䵽�� */
                    frameup = 0;
                    frame++;
                }
                else
                {
                    video_time_show(favi, &g_avix);                                                                     /* ��ʾ��ǰ����ʱ�� */
                    f_read(favi, psaibuf, g_avix.StreamSize + 8, &nr);                                                  /* ���psaibuf */
                    pbuf = psaibuf;
                }
                
                key = key_scan(0);
                if (key != 0)
                {
                    res = key;
                    break;
                }
                
                if (avi_get_streaminfo(pbuf + g_avix.StreamSize) != 0)                                                  /* ��ȡ��һ֡����־ */
                {
                    printf("g_frame error\r\n");
                    res = WKUP_PRES;
                    break;
                }
            }
            
            TIM7->CR1 &= ~(TIM_CR1_CEN);                                                                                /* �رջ�����ʱ��7���� */
            lcd_set_window(0, 0, lcddev.width, lcddev.height);                                                          /* �ָ����� */
            mjpegdec_free();                                                                                            /* �ͷ��ڴ� */
            f_close(favi);                                                                                              /* �ر��ļ� */
        }
    }
    
    myfree(SRAMIN, framebuf);
    myfree(SRAMIN, favi);
    myfree(SRAMIN, psaibuf);
    
    return res;
}

/**
 * @brief       ������Ƶ
 * @param       ��
 * @retval      ��
 */
void video_play(void)
{
    uint8_t res;
    DIR vdir;
    FILINFO *vfileinfo;
    uint8_t *pname;
    uint16_t totavinum;
    uint16_t curindex;
    uint32_t *voffsettbl;
    uint8_t key;
    uint32_t temp;
    
    while (f_opendir(&vdir, "0:/VIDEO") != FR_OK)                               /* ���VIDEO�ļ����Ƿ���� */
    {
        //text_show_string(60, 190, 240, 16, "VIDEO�ļ��д���!", 16, 0, RED);
        lcd_show_string(60, 190, 240, 16, 16, "VIDEO folder error!", RED);
    	delay_ms(200);
        lcd_fill(60, 190, 240, 206, WHITE);
        delay_ms(200);
    }
    
    totavinum = video_get_tnum("0:/VIDEO");                                     /* ����Ƿ�����Ƶ�ļ� */
    while (totavinum == 0)
    {
        //text_show_string(60, 190, 240, 16, "û����Ƶ�ļ�!", 16, 0, RED);
        lcd_show_string(60, 190, 240, 16, 16, "No video files!", RED);
        delay_ms(200);
        lcd_fill(60, 190, 240, 146, WHITE);
        delay_ms(200);
    }
    
    vfileinfo = (FILINFO *)mymalloc(SRAMIN, sizeof(FILINFO));                   /* �����ڴ� */
    pname = (uint8_t *)mymalloc(SRAMIN, 2 * _MAX_LFN + 1);
    voffsettbl = (uint32_t *)mymalloc(SRAMIN, totavinum  * 4);
    while ((vfileinfo == NULL) || (pname == NULL) || (voffsettbl == NULL))
    {
        //text_show_string(60, 190, 240, 16, "�ڴ����ʧ��!", 16, 0, RED);
        lcd_show_string(60, 190, 240, 16, 16, "Memory allocation failure!", RED);
        delay_ms(200);
        lcd_fill(60, 190, 240, 146, WHITE);
        delay_ms(200);
    }
    
    res = (uint8_t)f_opendir(&vdir, "0:/VIDEO");                                /* ��Ŀ¼ */
    if (res == 0)
    {
        curindex = 0;
        while (1)
        {
            temp = vdir.dptr;                                                   /* ��¼��ǰdptrƫ�� */
            res = (uint8_t)f_readdir(&vdir, vfileinfo);                         /* ��ȡ��һ���ļ� */
            if ((res != 0) || (vfileinfo->fname[0] == 0))                       /* �����ĩβ���˳� */
            {
                break;
            }
            
            res = exfuns_file_type(vfileinfo->fname);
            if ((res & 0xF0) == 0x60)                                           /* ��ͼƬ�ļ� */
            {
                voffsettbl[curindex] = temp;                                    /* ��¼���� */
                curindex++;
            }
        }
    }
    
    curindex = 0;
    res = (uint8_t)f_opendir(&vdir, "0:/VIDEO");                                /* ��Ŀ¼ */
    while (res == 0)
    {
        dir_sdi(&vdir, voffsettbl[curindex]);                                   /* �ı䵱ǰĿ¼���� */
        res = (uint8_t)f_readdir(&vdir, vfileinfo);                             /* ��ȡĿ¼����һ���ļ� */
        if ((res != 0) || (vfileinfo->fname[0] == 0))                           /* �����ĩβ���˳� */
        {
            break;
        }
        
        strcpy((char *)pname, "0:/VIDEO/");                                     /* ����·����Ŀ¼�� */
        strcat((char *)pname, (const char *)vfileinfo->fname);                  /* ���ļ������ں��� */
        
        lcd_clear(WHITE);
        video_bmsg_show((uint8_t *)vfileinfo->fname, curindex + 1, totavinum);  /* ��ʾ��Ƶ������Ϣ */
        
        key = video_play_mjpeg(pname);
        if (key == KEY0_PRES)   /* ��һ����Ƶ */
        {
            if (curindex != 0)
            {
                curindex--;
            }
            else
            {
                curindex = totavinum - 1;
            }
        }
        else if (key == WKUP_PRES)  /* ��һ����Ƶ */
        {
            curindex++;
            if (curindex >= totavinum)
            {
                curindex = 0;
            }
        }
        else
        {
            break;
        }
    }
    
    myfree(SRAMIN, vfileinfo);      /* �ͷ��ڴ� */
    myfree(SRAMIN, pname);
    myfree(SRAMIN, voffsettbl);
}
