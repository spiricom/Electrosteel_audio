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


float pedals[8][10] =
{
		{1.122462f, 1.0f, 1.0f, 1.0f, 1.0f, 1.122462f, 1.0f, 1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f, 1.0f, 1.059463f, 1.0f, 1.0f, 1.059463f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.122462f, 1.122462f, 1.0f, 1.0f, 1.0f},
		{0.66742f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.059463f},
		{1.0f, 0.943874f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.890899f, 1.0f},
		{1.0f, 1.0f, 1.059463f, 1.0f, 1.0f, 1.0f, 1.059463f, 1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 0.943874f, 1.0f, 1.0f, 1.0f, 0.943874f, 1.0f, 1.0f, 1.0f}
};



BOOL bufferCleared = TRUE;

int numBuffersToClearOnLoad = 2;
int numBuffersCleared = 0;

#define ATODB_TABLE_SIZE 512
#define ATODB_TABLE_SIZE_MINUS_ONE 511
float atodbTable[ATODB_TABLE_SIZE];

#define NUM_STRINGS 10

tADSR envelopes[10];
tADSR fenvelopes[10];
tMBSaw saws[10];
tMBSaw Ssaws[10];
tSimpleLivingString strings[10];
float theAmps[10];
tNoise myNoise;
tExpSmooth smoother[10];
tExpSmooth pitchSmoother[2];
tVZFilter filts[10];
tVZFilter noiseFilt;
tVZFilter noiseFilt2;
tExpSmooth pedalSmoothers[12];
tExpSmooth stringFreqSmoothers[10];

LEAF leaf;

/**********************************************/

float map(float value, float istart, float istop, float ostart, float ostop)
{
    return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
}


void audioInit(I2C_HandleTypeDef* hi2c, SAI_HandleTypeDef* hsaiOut, SAI_HandleTypeDef* hsaiIn)
{
	// Initialize LEAF.

	LEAF_init(&leaf, SAMPLE_RATE, AUDIO_FRAME_SIZE, small_memory, SMALL_MEM_SIZE, &randomNumber);

	tMempool_init (&mediumPool, medium_memory, MED_MEM_SIZE, &leaf);
	tMempool_init (&largePool, large_memory, LARGE_MEM_SIZE, &leaf);

	tCycle_init(&mySine[0], &leaf);
	tCycle_setFreq(&mySine[0], 220.0f);
	tCycle_init(&mySine[1], &leaf);
	tCycle_setFreq(&mySine[1], 220.0f);

	for (int i = 0; i < 10; i++)
	{
		tADSR_init(&envelopes[i], 4.0f, 2000.0f, 1.0f, 80.0f, &leaf);
		tADSR_setLeakFactor(&envelopes[i], 0.999999f),
		tMBSaw_initToPool(&saws[i], &mediumPool);
		tMBSaw_initToPool(&Ssaws[i], &mediumPool);
		tMBSaw_setFreq(&saws[i], 110.0f * ((float)i+1.0f));
		tSimpleLivingString_initToPool(&strings[i], 100.0f, 6000.0f, 0.999f, 0.9f, 0.01f, 0.01f, 0, &mediumPool);
		tExpSmooth_init(&smoother[i],0.0f, 0.002f, &leaf);
		tVZFilter_init(&filts[i], Lowpass, 8000.0f, 1.1f, &leaf);
		tVZFilter_init(&noiseFilt, BandpassPeak, 2000.0f, 1.5f, &leaf);
		tVZFilter_init(&noiseFilt2, Lowpass, 2000.0f, 0.9f, &leaf);
		tADSR_init(&fenvelopes[i], 5.0f, 1000.0f, 0.0f, 80.0f, &leaf);
		tExpSmooth_init(&stringFreqSmoothers[i],1.0f, 0.05f, &leaf);
	}
	for (int i = 0; i < 12; i++)
	{
		tExpSmooth_init(&pedalSmoothers[i],0.0f, 0.0008f, &leaf);
	}


	//loadingPreset = 1;
	//previousPreset = PresetNil;
	tNoise_init(&myNoise, WhiteNoise, &leaf);

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
float prevSamp = 0.0f;
float pedalValuesInt[12];
float pedalMin[12] = {1.0f, 1.0f, 1018.0f, 534.0f, 3109.0f, 616.0f, 312.0f, 410.0f, 1.0f, 353.0f, 3321.0f, 1.0f};
float pedalMax[12] = {2.0f, 1.0f, 1174.0f, 705.0f, 3377.0f, 840.0f, 733.0f, 892.0f, 2.0f, 746.0f, 3685.0f, 1.0f};
float pedalScaled[12];
int pedalOffset = 6;
uint maxVolumes[10];
float invMaxVolumes[10];
uint stringInputs[10];
float openStringFrequencies[NUM_STRINGS] = {123.470825f, 146.832384f, 164.813779f, 184.997211f, 207.652349f, 246.941651f, 329.627557f, 415.304698f, 311.126984f, 369.994423f};
float octave = 1.0f;

float stringMappedPositions[NUM_STRINGS];
float stringFrequencies[NUM_STRINGS];


// frets are measured at 3 7 12 and 19   need to redo these measurements with an accurately set capo
float fretMeasurements[4][2] ={
		{52628.0f, 53920.0f},
		{40008.0f, 41254.0f},
		{27462.0f, 28328.0f},
		{10040.0f, 10052.0f}
	};

float fretScaling[4] = {0.9f, 0.66666666666f, 0.5f, 0.25f};

void audioFrame(uint16_t buffer_offset)
{

	int i;
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
	uint32_t clipCatcher = 0;

	//if the codec isn't ready, keep the buffer as all zeros
	//otherwise, start computing audio!
	for (int j = 0; j < 2; j++)
	{
		stringPositions[j] =  ((uint16_t)SPI_RX[j * 2] << 8) + ((uint16_t)SPI_RX[(j * 2) + 1] & 0xff);
		if (stringPositions[j] == 65535)
		{
			stringMappedPositions[j] = 1.0f;
		}
		else
		{
			stringMappedPositions[j] = map((float)stringPositions[j], fretMeasurements[1][j], fretMeasurements[2][j], fretScaling[1], fretScaling[2]);
		}
	}

	for (int i = 0; i < 12; i++)
	{
		pedalValuesInt[i] = ((uint16_t)SPI_RX[(i * 2) + pedalOffset] << 8) + ((uint16_t)SPI_RX[(i * 2) + 1 + pedalOffset] & 0xff);
		tExpSmooth_setDest(&pedalSmoothers[i], LEAF_clip(0.0f, map(pedalValuesInt[i], pedalMin[i], pedalMax[i], -0.7f, 1.7f), 1.0f));
		pedalScaled[i] = tExpSmooth_tick(&pedalSmoothers[i]);
	}



	for (int i = 0; i < NUM_STRINGS; i++)
	{
		//interpolate ratios for each of the 10 strings
		float myMappedPos = stringMappedPositions[0];
		//float myMappedPos = LEAF_interpolation_linear(stringMappedPositions[1], stringMappedPositions[0], ((float)i) * 0.111111111111111f);

		//then apply those ratios to the fundamental frequencies
		float tempFreq = ((1.0 / myMappedPos) * openStringFrequencies[i] *
					(LEAF_interpolation_linear(1.0f, pedals[0][i], pedalScaled[2])) *
					(LEAF_interpolation_linear(1.0f, pedals[1][i], pedalScaled[3])) *
					(LEAF_interpolation_linear(1.0f, pedals[2][i], pedalScaled[4])) *
					(LEAF_interpolation_linear(1.0f, pedals[3][i], pedalScaled[5])) *
					(LEAF_interpolation_linear(1.0f, pedals[4][i], pedalScaled[6])) *
					(LEAF_interpolation_linear(1.0f, pedals[5][i], pedalScaled[7])) *
					(LEAF_interpolation_linear(1.0f, pedals[6][i], pedalScaled[9])) *
					(LEAF_interpolation_linear(1.0f, pedals[7][i], pedalScaled[10])));
		tExpSmooth_setDest(&stringFreqSmoothers[i], tempFreq);

	}
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







uint32_t audioTick(float* samples)
{
	uint32_t clips = 0;

	for (int i = 0; i < 12; i++)
	{
		pedalScaled[i] = tExpSmooth_tick(&pedalSmoothers[i]);
	}
	for (int i = 0; i < NUM_STRINGS; i++)
	{
		float theEnv = tADSR_tick(&fenvelopes[i]);
		stringFrequencies[i] = tExpSmooth_tick(&stringFreqSmoothers[i]);
		//tMBSaw_setFreq(&saws[i], (stringFrequencies[i]));
		//tMBSaw_setFreq(&Ssaws[i], stringFrequencies[i] * (1.0f + (2.0f *  theEnv)));
		tSimpleLivingString_setFreq(&strings[i], stringFrequencies[i]);
		tSimpleLivingString_setDampFreq(&strings[i], LEAF_clip(50.0f, (stringFrequencies[i] * 3.0f) + 6000.0f, 23000.0f));
		//tVZFilter_setFreq(&filts[i], stringFrequencies[i] * (1.0f + (6.0f * theEnv)));
	}
	samples[0] = 0.0f;

	float filtNoise = tVZFilter_tickEfficient(&noiseFilt, tNoise_tick(&myNoise));
	filtNoise += tVZFilter_tickEfficient(&noiseFilt2, filtNoise);
	for (int i = 0; i < 10; i++)
	{
		//tempSamp = tSaw_tick(&saws[i]) * tADSR_tick(&envelopes[i]);
		//tMBSaw_tick(&saws[i]);
		//tMBSaw_syncIn(&Ssaws[i], tMBSaw_syncOut(&saws[i]));
		//samples[0] +=  tMBSaw_tick(&saws[i]) * tADSR_tick(&envelopes[i]);
		//samples[0] +=  tVZFilter_tickEfficient(&filts[i], tMBSaw_tick(&saws[i]) * tADSR_tick(&envelopes[i]));
		samples[0] += (tSimpleLivingString_tick(&strings[i], (tExpSmooth_tick(&smoother[i]) * filtNoise) + (prevSamp * 0.0000f))) * tADSR_tick(&envelopes[i]);//tADSR_tick(&envelopes[i]);
	}
	prevSamp = samples[0];
	samples[0] *= .1f;
	samples[0] = tanhf(samples[0]);
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

int previousStringInputs[10];

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi == &hspi1)
	{
		for (int i = 0; i < 10; i++)
		{
			stringInputs[i] = (SPI_PLUCK_RX[i*2] << 8) + SPI_PLUCK_RX[(i*2)+1];
			if (maxVolumes[i] < stringInputs[i])
			{
				maxVolumes[i] = stringInputs[i];
				invMaxVolumes[i] = 1.0f / stringInputs[i];
			}

			if ((previousStringInputs[i] == 0) && (stringInputs[i] > 0))
			{
				//attack
				float amplitz = stringInputs[i] * invMaxVolumes[i];
				tExpSmooth_setVal(&smoother[i], 1.0f);
				tExpSmooth_setDest(&smoother[i], 0.0f);
				tADSR_on(&envelopes[i], amplitz);
				tADSR_on(&fenvelopes[i], amplitz);
				tVZFilter_setFreq(&noiseFilt, faster_mtof(amplitz * 25.0f + 60.0f));
				tVZFilter_setFreq(&noiseFilt2, faster_mtof(amplitz * 10.0f + 80.0f));

			}
			else if ((previousStringInputs[i] > 0) && (stringInputs[i] == 0))
			{
				tADSR_off(&envelopes[i]);
				tADSR_off(&fenvelopes[i]);
			}

			previousStringInputs[i] = stringInputs[i];
		}
	}
	HAL_SPI_Receive_DMA(&hspi1, SPI_PLUCK_RX, 20);
}

void HAL_SPI_RxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
}


void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
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
