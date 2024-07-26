/**
 ****************************************************************************************************
 * @file        audioplay.h
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

#ifndef __AVI_H
#define __AVI_H

#include "main.h"


/* ֡��С, �����Լ����ڴ��������������.
 * ����ڴ��,�������ô�һ��. ����ڴ���,����Сһ��.
 * һ���������ֵ�������ǵ�֡�����С����
 */
#define AVI_MAX_FRAME_SIZE  60*1024     /* ���֡��С,���ܳ���60KB */

/* �������� */
typedef enum
{
    AVI_OK = 0,             /* 0,�ɹ� */
    AVI_RIFF_ERR,           /* 1,RIFF ID��ȡʧ�� */
    AVI_AVI_ERR,            /* 2,AVI  ID��ȡʧ�� */
    AVI_LIST_ERR,           /* 3,LIST ID��ȡʧ�� */
    AVI_HDRL_ERR,           /* 4,HDRL ID��ȡʧ�� */
    AVI_AVIH_ERR,           /* 5,AVIH ID��ȡʧ�� */
    AVI_STRL_ERR,           /* 6,STRL ID��ȡʧ�� */
    AVI_STRH_ERR,           /* 7,STRH ID��ȡʧ�� */
    AVI_STRF_ERR,           /* 8,STRF ID��ȡʧ�� */
    AVI_MOVI_ERR,           /* 9,MOVI ID��ȡʧ�� */
    AVI_FORMAT_ERR,         /* 10,��ʽ���� */
    AVI_STREAM_ERR,         /* 11,������ */
} AVISTATUS;

#define AVI_RIFF_ID         0X46464952
#define AVI_AVI_ID          0X20495641
#define AVI_LIST_ID         0X5453494C
#define AVI_HDRL_ID         0X6C726468      /* ��Ϣ���־ */
#define AVI_MOVI_ID         0X69766F6D      /* ���ݿ��־ */
#define AVI_STRL_ID         0X6C727473      /* strl��־ */

#define AVI_AVIH_ID         0X68697661      /* avih�ӿ��AVI_HDRL_ID */
#define AVI_STRH_ID         0X68727473      /* strh(��ͷ)�ӿ��AVI_STRL_ID */
#define AVI_STRF_ID         0X66727473      /* strf(����ʽ)�ӿ��AVI_STRL_ID */
#define AVI_STRD_ID         0X64727473      /* strd�ӿ��AVI_STRL_ID (��ѡ��) */

#define AVI_VIDS_STREAM     0X73646976      /* ��Ƶ�� */
#define AVI_AUDS_STREAM     0X73647561      /* ��Ƶ�� */


#define AVI_VIDS_FLAG       0X6463          /* ��Ƶ����־ */
#define AVI_AUDS_FLAG       0X7762          /* ��Ƶ����־ */

#define AVI_FORMAT_MJPG     0X47504A4D


/* AVI ��Ϣ�ṹ�� */
/* ��һЩ��Ҫ������,���������,������� */
typedef __packed struct
{
    uint32_t SecPerFrame;       /* ��Ƶ֡���ʱ��(��λΪus) */
    uint32_t TotalFrame;        /* �ļ���֡�� */
    uint32_t Width;             /* ͼ��� */
    uint32_t Height;            /* ͼ��� */
    uint32_t SampleRate;        /* ��Ƶ������ */
    uint16_t Channels;          /* ������,һ��Ϊ2,��ʾ������ */
    uint16_t AudioBufSize;      /* ��Ƶ��������С */
    uint16_t AudioType;         /* ��Ƶ����:0X0001=PCM;0X0050=MP2;0X0055=MP3;0X2000=AC3; */
    uint16_t StreamID;          /* ������ID,StreamID=='dc'==0X6463 /StreamID=='wb'==0X7762 */
    uint32_t StreamSize;        /* ����С,������ż��,�����ȡ��Ϊ����,���1.��Ϊż�� */
    char *VideoFLAG;            /* ��Ƶ֡���,VideoFLAG="00dc"/"01dc" */
    char *AudioFLAG;            /* ��Ƶ֡���,AudioFLAG="00wb"/"01wb" */
} AVI_INFO;

extern AVI_INFO g_avix;           /* avi�ļ������Ϣ */

/* AVI ����Ϣ */
typedef struct
{
    uint32_t RiffID;            /* RiffID=='RIFF'==0X61766968 */
    uint32_t FileSize;          /* AVI�ļ���С(�����������8�ֽ�,ҲRIFFID��FileSize����������) */
    uint32_t AviID;             /* AviID=='AVI '==0X41564920 */
} AVI_HEADER;

/* AVI ����Ϣ */
typedef struct
{
    uint32_t FrameID;           /* ֡ID,FrameID=='RIFF'==0X61766968 */
    uint32_t FrameSize;         /* ֡��С */
} FRAME_HEADER;


/* LIST ����Ϣ */
typedef struct
{
    uint32_t ListID;            /* ListID=='LIST'==0X4c495354 */
    uint32_t BlockSize;         /* ���С(�����������8�ֽ�,ҲListID��BlockSize����������) */
    uint32_t ListType;          /* LIST�ӿ�����:hdrl(��Ϣ��)/movi(���ݿ�)/idxl(������,�Ǳ���,�ǿ�ѡ��) */
} LIST_HEADER;

/* avih �ӿ���Ϣ */
typedef struct
{
    uint32_t BlockID;           /* ���־:avih==0X61766968 */
    uint32_t BlockSize;         /* ���С(�����������8�ֽ�,Ҳ����BlockID��BlockSize����������) */
    uint32_t SecPerFrame;       /* ��Ƶ֡���ʱ��(��λΪus) */
    uint32_t MaxByteSec;        /* ������ݴ�����,�ֽ�/�� */
    uint32_t PaddingGranularity;/* ������������ */
    uint32_t Flags;             /* AVI�ļ���ȫ�ֱ�ǣ������Ƿ���������� */
    uint32_t TotalFrame;        /* �ļ���֡�� */
    uint32_t InitFrames;        /* Ϊ������ʽָ����ʼ֡�����ǽ�����ʽӦ��ָ��Ϊ0�� */
    uint32_t Streams;           /* �������������������,ͨ��Ϊ2 */
    uint32_t RefBufSize;        /* �����ȡ���ļ��Ļ����С��Ӧ���������Ŀ飩Ĭ�Ͽ�����1M�ֽ�!!! */
    uint32_t Width;             /* ͼ��� */
    uint32_t Height;            /* ͼ��� */
    uint32_t Reserved[4];       /* ���� */
} AVIH_HEADER;

/* strh ��ͷ�ӿ���Ϣ(strh��strl) */
typedef struct
{
    uint32_t BlockID;       /* ���־:strh==0X73747268 */
    uint32_t BlockSize;     /* ���С(�����������8�ֽ�,Ҳ����BlockID��BlockSize����������) */
    uint32_t StreamType;    /* ���������࣬vids(0X73646976):��Ƶ;auds(0X73647561):��Ƶ */
    uint32_t Handler;       /* ָ�����Ĵ����ߣ���������Ƶ��˵���ǽ�����,����MJPG/H264֮��� */
    uint32_t Flags;         /* ��ǣ��Ƿ�����������������ɫ���Ƿ�仯�� */
    uint16_t Priority;      /* �������ȼ������ж����ͬ���͵���ʱ���ȼ���ߵ�ΪĬ������ */
    uint16_t Language;      /* ��Ƶ�����Դ��� */
    uint32_t InitFrames;    /* Ϊ������ʽָ����ʼ֡�� */
    uint32_t Scale;         /* ������, ��Ƶÿ��Ĵ�С������Ƶ�Ĳ�����С */
    uint32_t Rate;          /* Scale/Rate=ÿ������� */
    uint32_t Start;         /* ��������ʼ���ŵ�λ�ã���λΪScale */
    uint32_t Length;        /* ������������������λΪScale */
    uint32_t RefBufSize;    /* ����ʹ�õĻ�������С */
    uint32_t Quality;       /* ��ѹ������������ֵԽ������Խ�� */
    uint32_t SampleSize;    /* ��Ƶ��������С */
    struct                  /* ��Ƶ֡��ռ�ľ��� */
    {
        short Left;
        short Top;
        short Right;
        short Bottom;
    } Frame;
} STRH_HEADER;

/* BMP�ṹ�� */
typedef struct
{
    uint32_t BmpSize;       /* bmp�ṹ���С,����(BmpSize����) */
    long Width;             /* ͼ��� */
    long Height;            /* ͼ��� */
    uint16_t  Planes;       /* ƽ����������Ϊ1 */
    uint16_t  BitCount;     /* ����λ��,0X0018��ʾ24λ */
    uint32_t  Compression;  /* ѹ�����ͣ�����:MJPG/H264�� */
    uint32_t  SizeImage;    /* ͼ���С */
    long XpixPerMeter;      /* ˮƽ�ֱ��� */
    long YpixPerMeter;      /* ��ֱ�ֱ��� */
    uint32_t  ClrUsed;      /* ʵ��ʹ���˵�ɫ���е���ɫ��,ѹ����ʽ�в�ʹ�� */
    uint32_t  ClrImportant; /* ��Ҫ����ɫ */
} BMP_HEADER;

/* ��ɫ�� */
typedef struct
{
    uint8_t  rgbBlue;       /* ��ɫ������(ֵ��ΧΪ0-255) */
    uint8_t  rgbGreen;      /* ��ɫ������(ֵ��ΧΪ0-255) */
    uint8_t  rgbRed;        /* ��ɫ������(ֵ��ΧΪ0-255) */
    uint8_t  rgbReserved;   /* ����������Ϊ0 */
} AVIRGBQUAD;

/* ����strh,�������Ƶ��,strf(����ʽ)ʹSTRH_BMPHEADER�� */
typedef struct
{
    uint32_t BlockID;       /* ���־,strf==0X73747266 */
    uint32_t BlockSize;     /* ���С(�����������8�ֽ�,Ҳ����BlockID�ͱ�BlockSize����������) */
    BMP_HEADER bmiHeader;   /* λͼ��Ϣͷ */
    AVIRGBQUAD bmColors[1]; /* ��ɫ�� */
} STRF_BMPHEADER;

/* ����strh,�������Ƶ��,strf(����ʽ)ʹSTRH_WAVHEADER�� */
typedef struct
{
    uint32_t BlockID;       /* ���־,strf==0X73747266 */
    uint32_t BlockSize;     /* ���С(�����������8�ֽ�,Ҳ����BlockID�ͱ�BlockSize����������) */
    uint16_t FormatTag;     /* ��ʽ��־:0X0001=PCM,0X0055=MP3 */
    uint16_t Channels;      /* ������,һ��Ϊ2,��ʾ������ */
    uint32_t SampleRate;    /* ��Ƶ������ */
    uint32_t BaudRate;      /* ������ */
    uint16_t BlockAlign;    /* ���ݿ�����־ */
    uint16_t Size;          /* �ýṹ��С */
} STRF_WAVHEADER;

#define	 MAKEWORD(ptr)	(uint16_t)(((uint16_t)*((uint8_t*)(ptr))<<8)|(uint16_t)*(uint8_t*)((ptr)+1))
#define  MAKEDWORD(ptr)	(uint32_t)(((uint16_t)*(uint8_t*)(ptr)|(((uint16_t)*(uint8_t*)(ptr+1))<<8)|\
                               (((uint16_t)*(uint8_t*)(ptr+2))<<16)|(((uint16_t)*(uint8_t*)(ptr+3))<<24)))


AVISTATUS avi_init(uint8_t *buf, uint32_t size);                /* ��ʼ��avi������ */
uint16_t avi_srarch_id(uint8_t *buf, uint32_t size, char *id);  /* ����ID,ID������4���ֽڳ��� */
AVISTATUS avi_get_streaminfo(uint8_t *buf);                     /* ��ȡ����Ϣ */


uint8_t Avih_Parser(uint8_t *buffer);
uint8_t Strl_Parser(uint8_t *buffer);
uint8_t Strf_Parser(uint8_t *buffer);

uint16_t Search_Movi(uint8_t *buffer);
uint16_t Search_Fram(uint8_t *buffer);
uint32_t ReadUnit(uint8_t *buffer, uint8_t index, uint8_t Bytes, uint8_t Format);

#endif
