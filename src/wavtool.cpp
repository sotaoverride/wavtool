#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cstring>
#include "logger.h"
//wav file header structure
struct WAVHeader {
	char chunkID[4];
	uint32_t chunkSize;
	char format[4];
	char subchunk1ID[4];
	uint32_t subchunk1Size;
	uint16_t audioFormat;
	uint16_t numChannels;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
	char subchunk2ID[4];
	uint32_t subchunk2Size;
};

int main(int argc, char *argv[]) {
	if (argc != 5) {
		printf("DTWAVTool input.wav output.wav source_channel dest_channel");
		return 1;
	}
	FILE *input = fopen(argv[1], "rb");
	char inputFileStr[] = "input";
	char outputFileStr[] = "output";
	FILE *output = fopen(argv[2], "wb");
	int srcChannel = atoi(argv[3]);
	int destChannel = atoi(argv[4]);

	if (!input || !output) {
		printf("ERROR opening files \n");
		LOG_ERROR("ERROR opening files %s \n", inputFileStr);
		return 1;
	}
	struct WAVHeader header;
	fread(&header, sizeof(header), 1, input);
	header.numChannels=destChannel;
	LOG_INFO("HEADER number of channels set to  %i \n", header.numChannels);
	LOG_IF_ERROR(header.numChannels != destChannel," HEADER number of channels %i, Dest Channel  %i \n", header.numChannels, destChannel);

	//Check if WAV file is valid
	if  (strncmp(header.chunkID, "RIFF", 4) != 0 ||
			strncmp(header.format, "WAVE", 4) !=0 ||
			strncmp(header.subchunk1ID, "fmt ", 4) !=0 ||
			strncmp(header.subchunk2ID, "data", 4) !=0) {
		printf("Invalid WAVE file.\n");
		LOG_ERROR("Invalid WAVE file %s %s %s %s  \n", header.chunkID, header.subchunk1ID, header.subchunk2ID, inputFileStr);
		return 1;
	}

	if (srcChannel < 1 || destChannel < 1) {
		printf("invalid number of channels");
		LOG_ERROR("invalid number of channels %s \n", inputFileStr);
		return 1;
	}	

	//write header to outputfile
	fwrite(&header, sizeof(header), 1, output);

	//copy the specified channel from source to destination
	int sampleSize = header.bitsPerSample / 8;
	int frameSize = sampleSize * header.numChannels;
	int frameSizeDest = sampleSize * destChannel;
	int bytesPerChannel = sampleSize;
	while (!feof(input)) {
		char frame[frameSize];
		char frameDest[sampleSize * destChannel]={8};
		int bytesRead = fread(frame, 1, frameSize, input);
		if (bytesRead < frameSize) break;

		//Extract and write to the desired destination channel
		for (int i=0; i< bytesRead; i += frameSize){
			memcpy(frameDest+i+(destChannel-1) * bytesPerChannel, frame + i + (srcChannel -1) * bytesPerChannel, bytesPerChannel);
			fwrite(frameDest, frameSizeDest, 1, output);
		}
	}
	fclose(input);
	fclose(output);
	return 0;
}
