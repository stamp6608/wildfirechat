/*
 * aes_crypt.c
 *
 *  Created on: 2013-7-17
 *      Author: zhouzhijie
 */

//#include "aes_crypt.h"
#include <string.h>
#include <stdlib.h>
#include <openssl/aes.h>
#include <time.h>

#define AES_KEY_LEN 16
#define AES_KEY_BITSET_LEN 128

int aes_cbc_encrypt_time_check(const unsigned char* pKey, unsigned int uiKeyLen
                    , const unsigned char* pInput, unsigned int uiInputLen
                    , unsigned char** ppOutput, unsigned int* pOutputLen)
{
    unsigned char keyBuf[AES_KEY_LEN] = {0};
    unsigned char iv[AES_KEY_LEN];
    
    AES_KEY aesKey;
    int ret;
    
    unsigned int uiPaddingLen;
    unsigned int uiTotalLen;
    
    if(pKey == NULL || uiKeyLen == 0 || pInput == NULL || uiInputLen == 0
       || pOutputLen == NULL || ppOutput == NULL)
        return -1;
    
    memcpy(keyBuf, pKey, (uiKeyLen > AES_KEY_LEN) ? AES_KEY_LEN : uiKeyLen);
    memcpy(iv, keyBuf, AES_KEY_LEN);
    
    ret = AES_set_encrypt_key(keyBuf, AES_KEY_BITSET_LEN, &aesKey);
    if(ret != 0) return -2;
    
    //second from 2018.1.1 0:0:0
	unsigned int timestamp = (unsigned int)time(NULL) - 1514736000;
    
    //hours from 2018.1.1 0:0:0
	unsigned int hours = timestamp / 3600;
    
    uiInputLen += 4;
    
    //padding
    uiPaddingLen = AES_KEY_LEN - (uiInputLen % AES_KEY_LEN);
    uiTotalLen = uiInputLen + uiPaddingLen;
    
    unsigned char *pData = (unsigned char*)malloc(sizeof(unsigned char) * uiTotalLen);
    unsigned short int si = 1;
    if ((si & 0xFF) == 1) { //small end
        memcpy(pData, &hours, 4);
    } else { //big end
        unsigned char byte0 = (hours & 0xFF);
        pData[0] = byte0;
        
        unsigned char byte1 = (hours & 0xFF00) >> 8;
        pData[1] = byte1;
        
        unsigned char byte2 = (hours & 0xFF0000) >> 16;
        pData[2] = byte2;
        
        unsigned char byte3 = (hours & 0xFF000000) >> 24;
        pData[3] = byte3;
    }
    
    memcpy(pData + 4, pInput, uiInputLen - 4);
    
    if(uiPaddingLen > 0) memset(pData+uiInputLen, uiPaddingLen, uiPaddingLen);
    
    *pOutputLen = uiTotalLen;
    *ppOutput = (unsigned char*)malloc(uiTotalLen);
    memset(*ppOutput, 0, uiTotalLen);
    
    AES_cbc_encrypt(pData, *ppOutput, uiTotalLen, &aesKey, iv, AES_ENCRYPT);
    free(pData);
    pData = NULL;
    return 0;
}

int aes_cbc_decrypt_time_check(const unsigned char* pKey, unsigned int uiKeyLen
                    , const unsigned char* pInput, unsigned int uiInputLen
                    , unsigned char** ppOutput, unsigned int* pOutputLen, bool checkTime)
{
    
    unsigned char keyBuf[AES_KEY_LEN] = {0};
    unsigned char iv[AES_KEY_LEN];
    
    AES_KEY aesKey;
    int ret;
    int uiPaddingLen;
    
    if(pKey == NULL || uiKeyLen == 0 || pInput == NULL || uiInputLen == 0 || pOutputLen == NULL
       || (uiInputLen%AES_KEY_LEN) != 0 || ppOutput == NULL)
        return -1;
    
    memcpy(keyBuf, pKey, (uiKeyLen > AES_KEY_LEN) ? AES_KEY_LEN : uiKeyLen);
    memcpy(iv, keyBuf, AES_KEY_LEN);
    
    ret = AES_set_decrypt_key(keyBuf, AES_KEY_BITSET_LEN, &aesKey);
    if(ret != 0) { return -2; }
    
    unsigned char *pBuf = (unsigned char *)malloc(uiInputLen);
    memset(pBuf, 0, uiInputLen);
    
    AES_cbc_encrypt(pInput, pBuf, uiInputLen, &aesKey, iv, AES_DECRYPT);
    
    uiPaddingLen = (pBuf)[uiInputLen - 1];
    if(uiPaddingLen > AES_KEY_LEN || uiPaddingLen <= 0) {
        free(pBuf);
        ppOutput = NULL;
        *pOutputLen = 0;
        return -3;
    }
    
    unsigned int *phours = (unsigned int *)pBuf;
    unsigned short int si = 1;
    if ((si & 0xFF) == 0) { //大端
        unsigned int hours = 0;
        hours += (hours & 0xFF) << 24;
        hours += (hours & 0xFF00) << 8;
        hours += (hours & 0xFF0000) >> 8;
        hours += (hours & 0xFF000000) >> 24;
    }
    //2018.1.1 0:0:0 以来的小时数
    if (checkTime) {
        unsigned int curts = ((unsigned int)time(NULL) - 1514736000) / 3600;
        if (((curts > *phours) && (curts - *phours > 24)) ||
            ((*phours > curts) && (*phours - curts > 24))) {
            free(pBuf);
            ppOutput = NULL;
            *pOutputLen = 0;
            return -4;
        }
    }
    
    *pOutputLen = uiInputLen - uiPaddingLen - 4;
    *ppOutput = (unsigned char* )malloc(*pOutputLen + 1);
    memset(*ppOutput, 0, *pOutputLen + 1);
    memcpy(*ppOutput, pBuf + 4, *pOutputLen);
    free(pBuf);

    return 0;
}

int aes_cbc_encrypt(const unsigned char* pKey, unsigned int uiKeyLen
		, const unsigned char* pInput, unsigned int uiInputLen
		, unsigned char** ppOutput, unsigned int* pOutputLen)
{
	unsigned char keyBuf[AES_KEY_LEN] = {0};
	unsigned char iv[AES_KEY_LEN];

    AES_KEY aesKey;
    int ret;

    unsigned int uiPaddingLen;
    unsigned int uiTotalLen;
    unsigned char* pData;

	if(pKey == NULL || uiKeyLen == 0 || pInput == NULL || uiInputLen == 0
	   || pOutputLen == NULL || ppOutput == NULL)
		return -1;
    

	memcpy(keyBuf, pKey, (uiKeyLen > AES_KEY_LEN) ? AES_KEY_LEN : uiKeyLen);
	memcpy(iv, keyBuf, AES_KEY_LEN);

    ret = AES_set_encrypt_key(keyBuf, AES_KEY_BITSET_LEN, &aesKey);
    if(ret != 0) return -2;

	//padding
	uiPaddingLen = AES_KEY_LEN - (uiInputLen % AES_KEY_LEN);
	uiTotalLen = uiInputLen + uiPaddingLen;
	pData = (unsigned char* )malloc(uiTotalLen);
	memcpy(pData, pInput, uiInputLen);

	if(uiPaddingLen > 0) memset(pData+uiInputLen, uiPaddingLen, uiPaddingLen);

    *pOutputLen = uiTotalLen;
	*ppOutput = (unsigned char* )malloc(uiTotalLen);
	memset(*ppOutput, 0, uiTotalLen);

    AES_cbc_encrypt(pData, *ppOutput, uiTotalLen, &aesKey, iv, AES_ENCRYPT);

	free(pData);
	return 0;
}

int aes_cbc_decrypt(const unsigned char* pKey, unsigned int uiKeyLen
		, const unsigned char* pInput, unsigned int uiInputLen
		, unsigned char** ppOutput, unsigned int* pOutputLen)
{

	unsigned char keyBuf[AES_KEY_LEN] = {0};
	unsigned char iv[AES_KEY_LEN];

    AES_KEY aesKey;
    int ret;
    int uiPaddingLen;

	if(pKey == NULL || uiKeyLen == 0 || pInput == NULL || uiInputLen == 0 || pOutputLen == NULL
			|| (uiInputLen%AES_KEY_LEN) != 0 || ppOutput == NULL)
		return -1;

	memcpy(keyBuf, pKey, (uiKeyLen > AES_KEY_LEN) ? AES_KEY_LEN : uiKeyLen);
	memcpy(iv, keyBuf, AES_KEY_LEN);

    ret = AES_set_decrypt_key(keyBuf, AES_KEY_BITSET_LEN, &aesKey);
    if(ret != 0) { return -2; }

	*ppOutput = (unsigned char* )malloc(uiInputLen);
	memset(*ppOutput, 0, uiInputLen);

    AES_cbc_encrypt(pInput, *ppOutput, uiInputLen, &aesKey, iv, AES_DECRYPT);

    uiPaddingLen = (*ppOutput)[uiInputLen - 1];
    if(uiPaddingLen > AES_KEY_LEN || uiPaddingLen <= 0) {
    	free(*ppOutput);
    	ppOutput = NULL;
    	return -3;
    }

    *pOutputLen = uiInputLen - uiPaddingLen;

	return 0;
}

int aes_ecb_encrypt(const unsigned char* pKey, unsigned int uiKeyLen
		, const unsigned char* pInput, unsigned int uiInputLen, int bFinal
		, unsigned char** ppOutput, unsigned int* pOutputLen)
{

	unsigned char keyBuf[AES_KEY_LEN] = {0};

    AES_KEY aesKey;
    int ret;

	unsigned int uiPaddingLen;
	unsigned int uiTotalLen;
	unsigned char* pData;

    unsigned int uiDone;
    unsigned char* pcInput;
    unsigned char* pcOutput;

	if(pKey == NULL || uiKeyLen == 0 || pInput == NULL || uiInputLen == 0 || pOutputLen == NULL
	   || (bFinal==0 && (uiInputLen%AES_KEY_LEN) != 0) || ppOutput == NULL)
		return -1;

	memcpy(keyBuf, pKey, (uiKeyLen > AES_KEY_LEN) ? AES_KEY_LEN : uiKeyLen);
    ret = AES_set_encrypt_key(keyBuf, AES_KEY_BITSET_LEN, &aesKey);

    if(ret != 0) { return -2;}

	//padding
	uiPaddingLen = (bFinal!=0) ? (AES_KEY_LEN - (uiInputLen % AES_KEY_LEN)) : 0;
	uiTotalLen = uiInputLen + uiPaddingLen;
	pData = (unsigned char* )malloc(uiTotalLen);
	memcpy(pData, pInput, uiInputLen);
	if(uiPaddingLen > 0) memset(pData+uiInputLen, uiPaddingLen, uiPaddingLen);

	*ppOutput = (unsigned char* )malloc(uiTotalLen);
	memset(*ppOutput, 0, uiTotalLen);

    uiDone = 0;
    pcInput = pData;
    pcOutput = *ppOutput;

    while(uiDone < uiTotalLen) {
    	AES_ecb_encrypt(pcInput, pcOutput, &aesKey, AES_ENCRYPT);
    	pcInput += AES_KEY_LEN;
    	pcOutput += AES_KEY_LEN;
    	uiDone += AES_KEY_LEN;
    }

    *pOutputLen = uiTotalLen;
	free(pData);
	return 0;
}


int aes_ecb_decrypt(const unsigned char* pKey, unsigned int uiKeyLen
		, const unsigned char* pInput, unsigned int uiInputLen, int bFinal
		, unsigned char** ppOutput, unsigned int* pOutputLen)
{
	unsigned char keyBuf[AES_KEY_LEN] = {0};

    AES_KEY aesKey;
    unsigned int uiDone;
    const unsigned char* pcInput;
    unsigned char* pcOutput;
    unsigned int uiPaddingLen;
	int ret;

	if(pKey == NULL || uiKeyLen == 0 || pInput == NULL || uiInputLen == 0 || pOutputLen == NULL
			|| (uiInputLen%AES_KEY_LEN) != 0 || ppOutput == NULL)
		return -1;

	memcpy(keyBuf, pKey, (uiKeyLen > AES_KEY_LEN) ? AES_KEY_LEN : uiKeyLen);

    ret = AES_set_decrypt_key(keyBuf, AES_KEY_BITSET_LEN, &aesKey);
    if(ret != 0) { return -2;}

	*ppOutput = (unsigned char* )malloc(uiInputLen);
	memset(*ppOutput, 0, uiInputLen);

    uiDone = 0;
    pcInput = pInput;
    pcOutput = *ppOutput;

    while(uiDone < uiInputLen) {
    	AES_ecb_encrypt(pcInput, pcOutput, &aesKey, AES_DECRYPT);
    	pcInput += AES_KEY_LEN;
    	pcOutput += AES_KEY_LEN;
    	uiDone += AES_KEY_LEN;
    }

    if(bFinal != 0)
    {
		uiPaddingLen = (*ppOutput)[uiInputLen - 1];
		if(uiPaddingLen > AES_KEY_LEN || uiPaddingLen <= 0) {
			free(*ppOutput);
			ppOutput = NULL;
			return -3;
		}

		*pOutputLen = uiInputLen - uiPaddingLen;
    }
    else
    	*pOutputLen = uiInputLen;

	return 0;
}
