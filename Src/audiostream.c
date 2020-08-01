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



/**********************************************/


void audioInit(I2C_HandleTypeDef* hi2c, SAI_HandleTypeDef* hsaiOut, SAI_HandleTypeDef* hsaiIn)
{
	// Initialize LEAF.

	LEAF_init(SAMPLE_RATE, AUDIO_FRAME_SIZE, small_memory, SMALL_MEM_SIZE, &randomNumber);

	tMempool_init (&mediumPool, medium_memory, MED_MEM_SIZE);
	tMempool_init (&largePool, large_memory, LARGE_MEM_SIZE);

	tCycle_init(&mySine[0]);
	tCycle_setFreq(&mySine[0], 220.0f);
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

	GPIO_InitTypeDef GPIO_InitStruct = {0};
}

void audioFrame(uint16_t buffer_offset)
{

	int i;

	uint32_t clipCatcher = 0;

	//if (!loadingPreset)
	//{
		//frameFunctions[currentPreset]();
	//}

	//if the codec isn't ready, keep the buffer as all zeros
	//otherwise, start computing audio!

	bufferCleared = TRUE;

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
		//if (!loadingPreset)
		//{
			//bufferCleared = 0;
		//}
	}
}




uint32_t audioTick(float* samples)
{
	uint32_t clips = 0;

	//tickFunctions[currentPreset](samples);
	samples[0] = tCycle_tick(&mySine[0]);
	samples[1] = samples[0];
	return clips;
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
