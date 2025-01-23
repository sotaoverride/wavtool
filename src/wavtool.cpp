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
	FILE *input = fopen(argv[1], "rb");
	char inputFileStr[] = "input";
	char outputFileStr[] = "output";
	FILE *output = fopen(argv[2], "wb");
	int tmpSrcChannels;
	if (!input || !output) {
		printf("ERROR opening files \n");
		LOG_ERROR("ERROR opening files %s \n", inputFileStr);
		return 1;
	}
	struct WAVHeader header;
	fread(&header, sizeof(header), 1, input);
	tmpSrcChannels = header.numChannels;
	header.numChannels=argc-3;
	int chanMapping[argc-3];
	LOG_INFO("HEADER number of channels set to  %i \n", header.numChannels);

	//Check if WAV file is valid
	if  (strncmp(header.chunkID, "RIFF", 4) != 0 ||
			strncmp(header.format, "WAVE", 4) !=0 ||
			strncmp(header.subchunk1ID, "fmt ", 4) !=0 ||
			strncmp(header.subchunk2ID, "data", 4) !=0) {
		printf("Invalid WAVE file.\n");
		LOG_ERROR("Invalid WAVE file %s %s %s %s  \n", header.chunkID, header.subchunk1ID, header.subchunk2ID, inputFileStr);
		return 1;
	}
	for (int i=0; i<argc-3; i++) {
		LOG_IF_ERROR(atoi(argv[i+2]) > header.numChannels -1," source has %i  number of channels , source channel given for mapping  %i \n", atoi(argv[i+2]), header.numChannels );
		chanMapping[i]=atoi(argv[i+2]); 
	}
	//write header to outputfile
	fwrite(&header, sizeof(header), 1, output);

	//copy the specified channel from source to destination
	int sampleSize = header.bitsPerSample / 8;
	int frameSize = sampleSize * tmpSrcChannels;
	int frameSizeDest = sampleSize * argc-3;
	int bytesPerChannel = sampleSize;
	while (!feof(input)) {
		char frame[frameSize];
		char frameDest[sampleSize * argc-3]={0};
		int bytesRead = fread(frame, 1, frameSize, input);
		if (bytesRead < frameSize) break;

		//Extract and write to the desired destination channel
		for(int i=0; i<argc-3; i++){
			if (chanMapping[i]>0)
			memcpy(frameDest + i * bytesPerChannel, frame + chanMapping[i] -1* bytesPerChannel, bytesPerChannel);
		}fwrite(frameDest, frameSizeDest, 1, output);
	}
	fclose(input);
	fclose(output);
	return 0;
}
