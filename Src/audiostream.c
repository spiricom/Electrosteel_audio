/*
 * audiostream.c
 *
 *  Created on: Aug 30, 2019
 *      Author: jeffsnyder
 */


/* Includes ------------------------------------------------------------------*/
#include "audiostream.h"
#include "main.h"
#include "leaf.h"
#include "codec.h"
#include "i2c.h"
#include "gpio.h"
#include "spi.h"

//the audio buffers are put in the D2 RAM area because that is a memory location that the DMA has access to.
int32_t audioOutBuffer[AUDIO_BUFFER_SIZE] __ATTR_RAM_D2;
int32_t audioInBuffer[AUDIO_BUFFER_SIZE] __ATTR_RAM_D2;

//#define DISPLAY_BLOCK_SIZE 512
//float audioDisplayBuffer[128];
//uint8_t displayBufferIndex = 0;
//float displayBlockVal = 0.0f;
//uint32_t displayBlockCount = 0;
char small_memory[SMALL_MEM_SIZE];
char medium_memory[MED_MEM_SIZE] __ATTR_RAM_D1;
char large_memory[LARGE_MEM_SIZE] __ATTR_SDRAM;
tMempool mediumPool;
tMempool largePool;

void audioFrame(uint16_t buffer_offset);
uint32_t audioTick(float* samples);
void buttonCheck(void);

HAL_StatusTypeDef transmit_status;
HAL_StatusTypeDef receive_status;

uint32_t codecReady = 0;

uint32_t frameCounter = 0;

int stringPositions[2];

tNoise myNoise;
tCycle mySine[2];
tEnvelopeFollower LED_envelope[4];

uint32_t clipCounter[4] = {0,0,0,0};
uint32_t clipped[4] = {0,0,0,0};
uint32_t clipHappened[4] = {0,0,0,0};


BOOL bufferCleared = TRUE;

int numBuffersToClearOnLoad = 2;
int numBuffersCleared = 0;

#define ATODB_TABLE_SIZE 512
#define ATODB_TABLE_SIZE_MINUS_ONE 511
float atodbTable[ATODB_TABLE_SIZE];


tADSR envelopes[10];
tSaw saws[10];
float theAmps[10];
/**********************************************/


void audioInit(I2C_HandleTypeDef* hi2c, SAI_HandleTypeDef* hsaiOut, SAI_HandleTypeDef* hsaiIn)
{
	// Initialize LEAF.

	LEAF_init(SAMPLE_RATE, AUDIO_FRAME_SIZE, small_memory, SMALL_MEM_SIZE, &randomNumber);

	tMempool_init (&mediumPool, medium_memory, MED_MEM_SIZE);
	tMempool_init (&largePool, large_memory, LARGE_MEM_SIZE);

	tCycle_init(&mySine[0]);
	tCycle_setFreq(&mySine[0], 220.0f);
	tCycle_init(&mySine[1]);
	tCycle_setFreq(&mySine[1], 220.0f);

	for (int i = 0; i < 10; i++)
	{
		tADSR_init(&envelopes[i], 5.0f, 4090.0f, 0.0f, 100.0f);
		tSaw_initToPool(&saws[i], &mediumPool);
		tSaw_setFreq(&saws[i], 110.0f * ((float)i+1.0f));
	}
	//loadingPreset = 1;
	//previousPreset = PresetNil;

	HAL_Delay(10);

	for (int i = 0; i < AUDIO_BUFFER_SIZE; i++)
	{
		audioOutBuffer[i] = 0;
	}

	HAL_Delay(1);

	// set up the I2S driver to send audio data to the codec (and retrieve input as well)
	transmit_status = HAL_SAI_Transmit_DMA(hsaiOut, (uint8_t *)&audioOutBuffer[0], AUDIO_BUFFER_SIZE);
	receive_status = HAL_SAI_Receive_DMA(hsaiIn, (uint8_t *)&audioInBuffer[0], AUDIO_BUFFER_SIZE);

	// with the CS4271 codec IC, the SAI Transmit and Receive must be happening before the chip will respond to
	// I2C setup messages (it seems to use the masterclock input as it's own internal clock for i2c data, etc)
	// so while we used to set up codec before starting SAI, now we need to set up codec afterwards, and set a flag to make sure it's ready

	//now to send all the necessary messages to the codec
	AudioCodec_init(hi2c);
	HAL_Delay(1);

}

void audioFrame(uint16_t buffer_offset)
{

	int i;
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
	uint32_t clipCatcher = 0;

	//if the codec isn't ready, keep the buffer as all zeros
	//otherwise, start computing audio!

	if (codecReady)
	{

		for (i = 0; i < (HALF_BUFFER_SIZE); i += 2)
		{
			float theSamples[2];
			theSamples[0] = ((float)(audioInBuffer[buffer_offset + i] << 8)) * INV_TWO_TO_31;
			theSamples[1] = ((float)(audioInBuffer[buffer_offset + i + 1] << 8)) * INV_TWO_TO_31;

			clipCatcher |= audioTick(theSamples);
			audioOutBuffer[buffer_offset + i] = (int32_t)(theSamples[1] * TWO_TO_23);
			audioOutBuffer[buffer_offset + i + 1] = (int32_t)(theSamples[0] * TWO_TO_23);
		}

	}
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
}


uint maxVolumes[10];
uint stringInputs[10];

uint32_t audioTick(float* samples)
{
	uint32_t clips = 0;
	for (int j = 0; j < 2; j++)
	{
		stringPositions[j] =  ((uint16_t)SPI_RX[j * 2] << 8) + ((uint16_t)SPI_RX[(j * 2) + 1] & 0xff);
	}
	//tCycle_setFreq(&mySine[0], mtof(((stringPositions[0] * INV_TWO_TO_16) * 24.0f) + 48.0f));

	//tCycle_setFreq(&mySine[1], mtof(((stringPositions[1] * INV_TWO_TO_16) * 24.0f) + 48.0f));

	samples[0] = 0.0f;

	uint string0 = (SPI_PLUCK_RX[0] << 8) + SPI_PLUCK_RX[1];

	//uint string0 = SPI_PLUCK_RX[0];
	if (string0 > 0)
	{
		//theAmps[0] = 1.0f;
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
	}
	else
	{
		//theAmps[0] = 0.0f;
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	}
	for (int i = 0; i < 10; i++)
	{
		samples[0] += tSaw_tick(&saws[i]) * theAmps[i];//tADSR_tick(&envelopes[i]);
	}
	samples[0] *= .10f;
	samples[1] = samples[0];
	return clips;
}




void midiIn (uint string, uint amplitude)
{
	if (amplitude > maxVolumes[string])
	{
		maxVolumes[string] = amplitude;
	}
	if (amplitude > 0)
	{
		theAmps[string] = 1.0f;
	}
	else
	{
		theAmps[string] = 0.0f;
	}
	if (theAmps[0] > 0.0f)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
	}
}

volatile int testInt = 0;

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi == &hspi1)
	{
		for (int i = 0; i < 10; i++)
		{
			stringInputs[i] = (SPI_PLUCK_RX[i*2] << 8) + SPI_PLUCK_RX[(i*2)+1];
			//if (maxVolumes[i] < stringInputs[i])
			//{
			//	maxVolumes[i] = stringInputs[i];
			//}

			theAmps[i] = (float)(stringInputs[i] > 0); //(float)stringInputs[i] / (float)maxVolumes[i];
		}


		//HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
		//uint tempAmp = (SPI_PLUCK_RX[1] << 8) + SPI_PLUCK_RX[2];
		//midiIn(SPI_PLUCK_RX[0], tempAmp);
	}
	HAL_SPI_Receive_DMA(&hspi1, SPI_PLUCK_RX, 20);
}

void HAL_SPI_RxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
	//if (hspi == &hspi1)
	//{


		//HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
		//int tempAmp = (SPI_PLUCK_RX[1] << 8) + SPI_PLUCK_RX[2];
		//midiIn(SPI_PLUCK_RX[0], tempAmp);
	//}
}


void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
	//setLED_Edit(1);
}

void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{

}

void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{

}


void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
	audioFrame(HALF_BUFFER_SIZE);
}

void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
	audioFrame(0);
}
