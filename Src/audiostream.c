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


uint32_t clipCounter[4] = {0,0,0,0};
uint32_t clipped[4] = {0,0,0,0};
uint32_t clipHappened[4] = {0,0,0,0};

#define DECAY_EXP_BUFFER_SIZE 512
float decayExpBufferSizeMinusOne = DECAY_EXP_BUFFER_SIZE - 1;
float decayExpBuffer[DECAY_EXP_BUFFER_SIZE];



float pedalsInCents[8][10] =
{
		{184.0f, 0.0f, 0.0f, -16.0f, 0.0f, 184.0f, 0.0f, 0.0f, 0.0f, -16.0f},
		{0.0f, -14.0f, 0.0f, 0.0f, 116.0f, 0.0f, 0.0f, 116.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, -16.0f, 0.0f, 184.0f, 184.0f, 0.0f, 0.0f, -16.0f},
		{-1200.0f, 0.0f, 0.0f, 0.0f, -1200.0f, -1200.0f, -1200.0f, -1200.0f, -1200.0f, -1200.0f},
		{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 186.0f},
		{0.0f, -134.0f, 0.0f, -14.0f, 0.0f, 0.0f, 0.0f, 0.0f, -204.0f, -14.0f},
		{0.0f, 0.0f, 78.0f, 0.0f, 0.0f, 0.0f, 78.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, -112.0f, 0.0f, 0.0f, 0.0f, -112.0f, 0.0f, 0.0f, 0.0f}
};

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
#define NUM_OSCS 3

tADSR4 envelopes[10];
tADSR4 fenvelopes[10];
tSawtooth saws[10];
tSawtooth Ssaws[10][NUM_OSCS];
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
tRosenbergGlottalPulse pulse[10];
tExpSmooth volumeSmoother;
tExpSmooth knobSmoothers[4];
tCycle mySine[10][2];


LEAF leaf;


float pedalValuesInt[12];
float pedalMin[12] = {1.0f, 1.0f, 1500.0f, 653.0f, 124.0f, 883.0f, 480.0f, 900.0f, 1.0f, 541.0f, 98.0f, 1.0f};
float pedalMax[12] = {2.0f, 1.0f, 1749.0f, 1059.0f, 663.0f, 1273.0f, 1215.0f, 1500.0f, 2.0f, 1264.0f, 273.0f, 1.0f};
float pedalScaled[12];
int pedalOffset = 6;
uint maxVolumes[10];
float invMaxVolumes[10];
uint stringInputs[10];
float openStringMidinotes[NUM_STRINGS] = {47.02f, 50.18f, 52.0f, 54.04f, 55.86f, 59.02f, 64.0f, 67.86f, 62.88f, 66.04f};
float openStringFrequencies[NUM_STRINGS] = {123.470825f, 146.832384f, 164.813779f, 184.997211f, 207.652349f, 246.941651f, 329.627557f, 415.304698f, 311.126984f, 369.994423f};
float octave = 1.0f;

float stringMappedPositions[NUM_STRINGS];
float stringFrequencies[NUM_STRINGS];
float detunes[NUM_OSCS] = {0.9992f, 1.0f, 1.0002f};

float prevSamp[NUM_STRINGS];
float sympathetic = 0.000001f;
float volumePedal = 0.0f;
float knobScaled[4];

// frets are measured at 3 7 12 and 19   need to redo these measurements with an accurately set capo
float fretMeasurements[4][2] ={
		{65535.0f, 65535.0f},
		{27800.0f, 27800.0f},
		{27336.0f, 28186.0f},
		{9460.0f, 9849.0f}
	};

float fretScaling[4] = {1.0f, 0.5f, 0.5f, 0.25f};


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


	LEAF_generate_exp(decayExpBuffer, 0.001f, 0.0f, 1.0f, -0.0008f, DECAY_EXP_BUFFER_SIZE); // exponential decay buffer falling from 1 to 0
	for (int i = 0; i < 10; i++)
	{
		tADSR4_init(&envelopes[i], 10.0f, 500.0f, 0.90f, 200.0f, decayExpBuffer, DECAY_EXP_BUFFER_SIZE, &leaf);
		tADSR4_setLeakFactor(&envelopes[i], 0.999999f);
		tRosenbergGlottalPulse_init(&pulse[i], &leaf);
		tRosenbergGlottalPulse_setOpenLengthAndPulseLength(&pulse[i], 0.5f, 0.4f);
		tSawtooth_initToPool(&saws[i], &mediumPool);
		for (int j = 0; j < NUM_OSCS; j++)
		{
			tSawtooth_initToPool(&Ssaws[i][j], &mediumPool);
		}
		tSawtooth_setFreq(&saws[i], 110.0f * ((float)i+1.0f));
		tSimpleLivingString_initToPool(&strings[i], 100.0f, 6000.0f, 0.999f, 0.9f, 0.01f, 0.01f, 0, &mediumPool);
		tExpSmooth_init(&smoother[i],0.0f, 0.002f, &leaf);
		tVZFilter_init(&filts[i], Lowpass, 8000.0f, 6.1f, &leaf);
		tVZFilter_init(&noiseFilt, BandpassPeak, 2000.0f, 1.5f, &leaf);
		tVZFilter_init(&noiseFilt2, Lowpass, 2000.0f, 0.9f, &leaf);
		tADSR4_init(&fenvelopes[i], 10.0f,  12000.0f, 0.0f, 200.0f, decayExpBuffer, DECAY_EXP_BUFFER_SIZE, &leaf);
		tExpSmooth_init(&stringFreqSmoothers[i],1.0f, 0.04f, &leaf);

		for (int j = 0; j < 2; j++)
		{
			tCycle_init(&mySine[i][j], &leaf);
			tCycle_setFreq(&mySine[i][j], (randomNumber() * 0.1f) + 0.001f);
		}
		openStringFrequencies[i] = mtof(openStringMidinotes[i]);
	}
	for (int i = 0; i < 12; i++)
	{
		tExpSmooth_init(&pedalSmoothers[i],0.0f, 0.0005f, &leaf);
	}
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < NUM_STRINGS; j++)
		{
			pedals[i][j] = pow(2.0, (pedalsInCents[i][j] / 1200.0));
		}
	}
	tExpSmooth_init(&volumeSmoother,0.0f, 0.0005f, &leaf);
	for (int i = 0; i < 4; i++)
	{
		tExpSmooth_init(&knobSmoothers[i],0.0f, 0.0005f, &leaf);
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
			stringMappedPositions[j] = map((float)stringPositions[j], fretMeasurements[0][j], fretMeasurements[1][j], fretScaling[0], fretScaling[1]);
		}
	}

	if ((SPI_LEVERS[30] == 254) && (SPI_LEVERS[31] == 253))
	{
		for (int i = 0; i < 9; i++)
		{
			pedalValuesInt[i] = ((uint16_t)SPI_LEVERS[(i * 2)] << 8) + ((uint16_t)SPI_LEVERS[(i * 2) + 1] & 0xff);
			tExpSmooth_setDest(&pedalSmoothers[i], LEAF_clip(0.0f, ((pedalValuesInt[i] * 0.0002490234375f) - 0.01f), 1.0f)); //   divided by 4096 multiplied by 1.02 and subtracting 0.01 to push it a little beyond the edges.
			//pedalScaled[i] = tExpSmooth_tick(&pedalSmoothers[i]);
		}
		for (int i = 0; i < 4; i++)
		{
			tExpSmooth_setDest(&knobSmoothers[i], (SPI_LEVERS[i+21] * 0.0078125)); //   divided by 128
			//knobScaled[i] = tExpSmooth_tick(&knobSmoothers[i]);
		}
		octave = powf(2.0f,((int32_t) SPI_LEVERS[20] - 1 ));

		uint16_t volumePedalInt = ((uint16_t)SPI_LEVERS[25] << 8) + ((uint16_t)SPI_LEVERS[26] & 0xff);
		volumePedal = volumePedalInt * 0.00006103515625f;
		tExpSmooth_setDest(&volumeSmoother,volumePedal);
	}
	for (int i = 0; i < NUM_STRINGS; i++)
	{
		//interpolate ratios for each of the 10 strings
		float myMappedPos = stringMappedPositions[1];
		//float myMappedPos = LEAF_interpolation_linear(stringMappedPositions[1], stringMappedPositions[0], ((float)i) * 0.111111111111111f);

		//then apply those ratios to the fundamental frequencies
		float tempFreq = ((1.0 / myMappedPos) * openStringFrequencies[i] * octave *
					(LEAF_interpolation_linear(1.0f, pedals[0][i], pedalScaled[0])) *
					(LEAF_interpolation_linear(1.0f, pedals[1][i], pedalScaled[1])) *
					(LEAF_interpolation_linear(1.0f, pedals[2][i], pedalScaled[2])) *
					(LEAF_interpolation_linear(1.0f, pedals[3][i], pedalScaled[3])) *
					(LEAF_interpolation_linear(1.0f, pedals[4][i], pedalScaled[5])) *
					(LEAF_interpolation_linear(1.0f, pedals[5][i], pedalScaled[6])) *
					(LEAF_interpolation_linear(1.0f, pedals[6][i], pedalScaled[7])) *
					(LEAF_interpolation_linear(1.0f, pedals[7][i], pedalScaled[8])));
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
	for (int i = 0; i < 4; i++)
	{
		knobScaled[i] = tExpSmooth_tick(&knobSmoothers[i]);
	}
	float volumeSmoothed = tExpSmooth_tick(&volumeSmoother);
	for (int i = 0; i < NUM_STRINGS; i++)
	{
		float theEnv = tADSR4_tick(&fenvelopes[i]);
		stringFrequencies[i] = tExpSmooth_tick(&stringFreqSmoothers[i]);


		//tSawtooth_setFreq(&saws[i], stringFrequencies[i] * 7.0f);// + (tMBSaw_tick(&saws[i]) * 100.0f));
		//float fm = tSawtooth_tick(&saws[i]);
		for (int j = 0; j < NUM_OSCS; j++)
		{
			tSawtooth_setFreq(&Ssaws[i][j], stringFrequencies[i] * detunes[j]);// + (fm * 150.0f));// * (1.0f + (7.0f *  theEnv)));
		}
		//tSimpleLivingString_setFreq(&strings[i], stringFrequencies[i]);
		//tSimpleLivingString_setDampFreq(&strings[i], LEAF_clip(50.0f, (stringFrequencies[i] * 3.0f) + 6000.0f, 23000.0f));
		tVZFilter_setFreq(&filts[i], LEAF_clip(30.0f, stringFrequencies[i]*((1.0f-knobScaled[0]) + 0.1f) * 16.0f, 10000.0f));
		//tVZFilter_setFreq(&filts[i], LEAF_clip(10.0f, stringFrequencies[i] * (1.0f + (16 .0f * theEnv)), 5000.0f));
		//tRosenbergGlottalPulse_setFreq(&pulse[i], stringFrequencies[i] * 0.999f);
		//float lfo1 = tCycle_tick(&mySine[i][0]) * 0.3f + 0.5f;
		//tRosenbergGlottalPulse_setOpenLengthAndPulseLength(&pulse[i],(tCycle_tick(&mySine[i][1]) * 0.2f + 0.5f) * lfo1, lfo1);
	}
	samples[0] = 0.0f;

	//float filtNoise = tVZFilter_tickEfficient(&noiseFilt, tNoise_tick(&myNoise));
	//filtNoise += tVZFilter_tickEfficient(&noiseFilt2, filtNoise);
	for (int i = 0; i < 10; i++)
	{
		//tempSamp = tSaw_tick(&saws[i]) * tADSR_tick(&envelopes[i]);
		float tempSamp = 0.0f;
		//tMBSaw_syncIn(&Ssaws[i], tMBSaw_syncOut(&saws[i]));
		//samples[0] +=  tMBSaw_tick(&saws[i]) * tADSR_tick(&envelopes[i]);
		float env = tADSR4_tick(&envelopes[i]);
		for (int j = 0; j < NUM_OSCS; j++)
		{
			tempSamp += tSawtooth_tick(&Ssaws[i][j]) * 0.5f;
		}
		tempSamp = tVZFilter_tickEfficient(&filts[i], tempSamp * env);
		//tempSamp = tVZFilter_tickEfficient(&filts[i], tRosenbergGlottalPulse_tickHQ(&pulse[i]) * env);
		//tempSamp += tRosenbergGlottalPulse_tick(&pulse[i]) * env;
		samples[0] += tempSamp;
		//samples[0] +=  tSimpleLivingString_tick(&strings[i], tempSamp  * 0.4f);
		//float tempSamp = (tSimpleLivingString_tick(&strings[i], (tExpSmooth_tick(&smoother[i]) * filtNoise) + (prevSamp[i] * sympathetic))) * tADSR_tick(&envelopes[i]);
		//samples[0] += tempSamp;
		//prevSamp[i] = 0.0f;
		//for (int j = 0; j < 10; j++)
		{
			//if (j != i) //put sympathetic resonance in all strings but yourself
			{
				//prevSamp[j]+=tempSamp;
			}
		}
	}
	samples[0] *= .1f * volumeSmoothed;
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

		if ((SPI_PLUCK_RX[0] == 254) && (SPI_PLUCK_RX[21] == 253))
		{

			for (int i = 0; i < 10; i++)
			{

				stringInputs[i] = (SPI_PLUCK_RX[(i*2)+1] << 8) + SPI_PLUCK_RX[(i*2)+2];

				if (maxVolumes[i] < stringInputs[i])
				{
					maxVolumes[i] = stringInputs[i];
					invMaxVolumes[i] = 1.0f / stringInputs[i];
				}

				if ((previousStringInputs[i] == 0) && (stringInputs[i] > 0))
				{
					//attack
					float amplitz = stringInputs[i] * invMaxVolumes[i];
					//tExpSmooth_setVal(&smoother[i], 1.0f);
					//tExpSmooth_setDest(&smoother[i], 0.0f);
					tADSR4_on(&envelopes[i], amplitz);
					//tADSR4_on(&fenvelopes[i], amplitz);
					//tVZFilter_setFreq(&noiseFilt, faster_mtof(amplitz * 25.0f + 60.0f));
					//tVZFilter_setFreq(&noiseFilt2, faster_mtof(amplitz * 10.0f + 80.0f));

				}
				else if ((previousStringInputs[i] > 0) && (stringInputs[i] == 0))
				{
					tADSR4_off(&envelopes[i]);
					//tADSR4_off(&fenvelopes[i]);
				}

				previousStringInputs[i] = stringInputs[i];
			}
		}
		/*
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
				tADSR4_on(&envelopes[i], amplitz);
				tADSR4_on(&fenvelopes[i], amplitz);
				tVZFilter_setFreq(&noiseFilt, faster_mtof(amplitz * 25.0f + 60.0f));
				tVZFilter_setFreq(&noiseFilt2, faster_mtof(amplitz * 10.0f + 80.0f));

			}
			else if ((previousStringInputs[i] > 0) && (stringInputs[i] == 0))
			{
				tADSR4_off(&envelopes[i]);
				tADSR4_off(&fenvelopes[i]);
			}

			previousStringInputs[i] = stringInputs[i];
		}
		*/
		HAL_SPI_Receive_DMA(&hspi1, SPI_PLUCK_RX, 22);
	}

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
