/**
 ****************************************************************************************************
 * @file        audioplay.c
 * @author      ALIENTEK
 * @brief       AVI video format parsing code
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

#include "../../ATK_Middlewares/MJPEG/avi.h"
#include "../../ATK_Middlewares/MJPEG/mjpeg.h"

#include "usart.h"


AVI_INFO g_avix;        /* avi�ļ������Ϣ */
char *const AVI_VIDS_FLAG_TBL[2] = {"00dc", "01dc"};  /* ��Ƶ�����־�ַ���,00dc/01dc */
char *const AVI_AUDS_FLAG_TBL[2] = {"00wb", "01wb"};  /* ��Ƶ�����־�ַ���,00wb/01wb */

/**
 * @brief       avi�����ʼ��
 * @param       buf  : ���뻺����
 * @param       size : ��������С
 * @retval      res
 *    @arg      OK,avi�ļ������ɹ�
 *    @arg      ����,�������
 */
AVISTATUS avi_init(uint8_t *buf, uint32_t size)
{
    uint16_t offset;
    uint8_t *tbuf;
    
    AVISTATUS res = AVI_OK;
    
    AVI_HEADER *aviheader;
    LIST_HEADER *listheader;
    AVIH_HEADER *avihheader;
    STRH_HEADER *strhheader;

    STRF_BMPHEADER *bmpheader;
    STRF_WAVHEADER *wavheader;

    tbuf = buf;
    aviheader = (AVI_HEADER *)buf;
    if (aviheader->RiffID != AVI_RIFF_ID)
    {
        return AVI_RIFF_ERR;        /* RIFF ID���� */
    }

    if (aviheader->AviID != AVI_AVI_ID)
    {
        return AVI_AVI_ERR;         /* AVI ID���� */
    }

    buf += sizeof(AVI_HEADER);      /* ƫ�� */
    listheader = (LIST_HEADER *)(buf);
    if (listheader->ListID != AVI_LIST_ID)
    {
        return AVI_LIST_ERR;        /* LIST ID���� */
    }

    if (listheader->ListType != AVI_HDRL_ID)
    {
        return AVI_HDRL_ERR;        /* HDRL ID���� */
    }

    buf += sizeof(LIST_HEADER);     /* ƫ�� */
    avihheader = (AVIH_HEADER *)(buf);
    if (avihheader->BlockID != AVI_AVIH_ID)
    {
        return AVI_AVIH_ERR;        /* AVIH ID���� */
    }

    g_avix.SecPerFrame = avihheader->SecPerFrame;   /* �õ�֡���ʱ�� */
    g_avix.TotalFrame = avihheader->TotalFrame;     /* �õ���֡�� */
    buf += avihheader->BlockSize + 8;               /* ƫ�� */
    listheader = (LIST_HEADER *)(buf);
    if (listheader->ListID != AVI_LIST_ID)
    {
        return AVI_LIST_ERR;        /* LIST ID���� */
    }

    if (listheader->ListType != AVI_STRL_ID)
    {
        return AVI_STRL_ERR;        /* STRL ID���� */
    }

    strhheader = (STRH_HEADER *)(buf + 12);
    if (strhheader->BlockID != AVI_STRH_ID)
    {
        return AVI_STRH_ERR;        /* STRH ID���� */
    }

    if (strhheader->StreamType == AVI_VIDS_STREAM)  /* ��Ƶ֡��ǰ */
    {
        if (strhheader->Handler != AVI_FORMAT_MJPG)
        {
            return AVI_FORMAT_ERR;  /* ��MJPG��Ƶ��,��֧�� */
        }

        g_avix.VideoFLAG = AVI_VIDS_FLAG_TBL[0];    /* ��Ƶ�����  "00dc" */
        g_avix.AudioFLAG = AVI_AUDS_FLAG_TBL[1];    /* ��Ƶ�����  "01wb" */
        bmpheader = (STRF_BMPHEADER *)(buf + 12 + strhheader->BlockSize + 8);   /* strf */
        if (bmpheader->BlockID != AVI_STRF_ID)
        {
            return AVI_STRF_ERR;    /* STRF ID���� */
        }

        g_avix.Width = bmpheader->bmiHeader.Width;
        g_avix.Height = bmpheader->bmiHeader.Height;
        buf += listheader->BlockSize + 8;       /* ƫ�� */
        listheader = (LIST_HEADER *)(buf);
        if (listheader->ListID != AVI_LIST_ID)  /* �ǲ�������Ƶ֡����Ƶ�ļ� */
        {
            g_avix.SampleRate = 0;              /* ��Ƶ������ */
            g_avix.Channels = 0;                /* ��Ƶͨ���� */
            g_avix.AudioType = 0;               /* ��Ƶ��ʽ */

        }
        else
        {
            if (listheader->ListType != AVI_STRL_ID)
            {
                return AVI_STRL_ERR;    /* STRL ID���� */
            }

            strhheader = (STRH_HEADER *)(buf + 12);
            if (strhheader->BlockID != AVI_STRH_ID)
            {
                return AVI_STRH_ERR;    /* STRH ID���� */
            }

            if (strhheader->StreamType != AVI_AUDS_STREAM)
            {
                return AVI_FORMAT_ERR;  /* ��ʽ���� */
            }

            wavheader = (STRF_WAVHEADER *)(buf + 12 + strhheader->BlockSize + 8);   /* strf */
            if (wavheader->BlockID != AVI_STRF_ID)
            {
                return AVI_STRF_ERR;    /* STRF ID���� */
            }

            g_avix.SampleRate = wavheader->SampleRate;      /* ��Ƶ������ */
            g_avix.Channels = wavheader->Channels;          /* ��Ƶͨ���� */
            g_avix.AudioType = wavheader->FormatTag;        /* ��Ƶ��ʽ */
        }
    }
    else if (strhheader->StreamType == AVI_AUDS_STREAM)     /* ��Ƶ֡��ǰ */
    { 
        g_avix.VideoFLAG = AVI_VIDS_FLAG_TBL[1];            /* ��Ƶ�����  "01dc" */
        g_avix.AudioFLAG = AVI_AUDS_FLAG_TBL[0];            /* ��Ƶ�����  "00wb" */
        wavheader = (STRF_WAVHEADER *)(buf + 12 + strhheader->BlockSize + 8);   /* strf */
        if (wavheader->BlockID != AVI_STRF_ID)
        {
            return AVI_STRF_ERR;                            /* STRF ID���� */
        }
 
        g_avix.SampleRate = wavheader->SampleRate;          /* ��Ƶ������ */
        g_avix.Channels = wavheader->Channels;              /* ��Ƶͨ���� */
        g_avix.AudioType = wavheader->FormatTag;            /* ��Ƶ��ʽ */
        buf += listheader->BlockSize + 8;                   /* ƫ�� */
        listheader = (LIST_HEADER *)(buf);
        if (listheader->ListID != AVI_LIST_ID)
        {
            return AVI_LIST_ERR;    /* LIST ID���� */
        }

        if (listheader->ListType != AVI_STRL_ID)
        {
            return AVI_STRL_ERR;    /* STRL ID���� */
        }

        strhheader = (STRH_HEADER *)(buf + 12);
        if (strhheader->BlockID != AVI_STRH_ID)
        {
            return AVI_STRH_ERR;    /* STRH ID���� */
        }

        if (strhheader->StreamType != AVI_VIDS_STREAM)
        {
            return AVI_FORMAT_ERR;  /* ��ʽ���� */
        }

        bmpheader = (STRF_BMPHEADER *)(buf + 12 + strhheader->BlockSize + 8);   /* strf */
        if (bmpheader->BlockID != AVI_STRF_ID)
        {
            return AVI_STRF_ERR;    /* STRF ID���� */
        }

        if (bmpheader->bmiHeader.Compression != AVI_FORMAT_MJPG)
        {
            return AVI_FORMAT_ERR;  /* ��ʽ���� */
        }

        g_avix.Width = bmpheader->bmiHeader.Width;
        g_avix.Height = bmpheader->bmiHeader.Height;
    }

    offset = avi_srarch_id(tbuf, size, "movi");     /* ����movi ID */
    if (offset == 0)
    {
        return AVI_MOVI_ERR;        /* MOVI ID���� */
    }

    if (g_avix.SampleRate)          /* ����Ƶ��,�Ų��� */
    {
        tbuf += offset;
        offset = avi_srarch_id(tbuf, size, g_avix.AudioFLAG);   /* ������Ƶ����� */
        if (offset == 0)
        {
            return AVI_STREAM_ERR;  /* ������ */
        }
        tbuf += offset + 4;
        g_avix.AudioBufSize = *((uint16_t *)tbuf);              /* �õ���Ƶ��buf��С. */
    }

    printf("avi init ok\r\n");
    printf("g_avix.SecPerFrame:%d\r\n", g_avix.SecPerFrame);
    printf("g_avix.TotalFrame:%d\r\n", g_avix.TotalFrame);
    printf("g_avix.Width:%d\r\n", g_avix.Width);
    printf("g_avix.Height:%d\r\n", g_avix.Height);
    printf("g_avix.AudioType:%d\r\n", g_avix.AudioType);
    printf("g_avix.SampleRate:%d\r\n", g_avix.SampleRate);
    printf("g_avix.Channels:%d\r\n", g_avix.Channels);
    printf("g_avix.AudioBufSize:%d\r\n", g_avix.AudioBufSize);
    printf("g_avix.VideoFLAG:%s\r\n", g_avix.VideoFLAG);
    printf("g_avix.AudioFLAG:%s\r\n", g_avix.AudioFLAG);

    return res;
}

/**
 * @brief       ���� ID
 * @param       buf  : ���黺����
 * @param       size : �����С
 * @param       id   : Ҫ���ҵ�id,������4�ֽڳ���
 * @retval      0,����Ӧ��ʧ��
 *              ����:movi IDƫ����
 */
uint16_t avi_srarch_id(uint8_t *buf, uint32_t size, char *id)
{
    uint32_t i;
    uint32_t idsize = 0;
    size -= 4;
    for (i = 0; i < size; i++)
    {
        if ((buf[i] == id[0]) &&
            (buf[i + 1] == id[1]) &&
            (buf[i + 2] == id[2]) &&
            (buf[i + 3] == id[3]))
        {
            idsize = MAKEDWORD(buf + i + 4);    /* �õ�֡��С,�������16�ֽ�,�ŷ���,��������Ч���� */

            if (idsize > 0X10)return i;         /* �ҵ�"id"���ڵ�λ�� */
        }
    }

    return 0;
}

/**
 * @brief       �õ�stream����Ϣ
 * @param       buf:����ʼ��ַ(������01wb/00wb/01dc/00dc��ͷ)
 * @retval      ��
 */
AVISTATUS avi_get_streaminfo(uint8_t *buf)
{
    g_avix.StreamID = MAKEWORD(buf + 2);    /* �õ������� */
    g_avix.StreamSize = MAKEDWORD(buf + 4); /* �õ�����С */

    if (g_avix.StreamSize > AVI_MAX_FRAME_SIZE)    /* ֡��С̫����,ֱ�ӷ��ش��� */
    {
        printf("FRAME SIZE OVER:%d\r\n", g_avix.StreamSize);
        g_avix.StreamSize = 0;
        return AVI_STREAM_ERR;
    }

    if (g_avix.StreamSize % 2)
    {
        g_avix.StreamSize++;    /* ������1(g_avix.Stre amSize,������ż��) */
    }

    if (g_avix.StreamID == AVI_VIDS_FLAG || g_avix.StreamID == AVI_AUDS_FLAG)
    {
        return AVI_OK;
    }

    return AVI_STREAM_ERR;
}
