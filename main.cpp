#include <cstdlib>
#include <iostream>
#include "RF24/RF24.h"
#include "FWUpdater.h"
#include <fstream>
#include "HomeConfigParser.h"

using namespace std;

// spi device, spi speed, ce gpio pin
RF24 radio(25, 0);

static const uint8_t rx_address[3] = { 0xe7, 0xe7, 0xe7 };
static const uint8_t tx_address[3] = { 0xe7, 0xe7, 0xe7 };

void* memcpy_index(void *s1, const void *s2, size_t index, size_t n) {
	s2 = ((char*) s2) + index;
	return memcpy(s1, s2, n);
}

static void
parseCmdLineArgs(
		int						argc,
		char **					argv,
		const char *&			homeConfigFile,
		const char *&			homeDeviceName,
		const char *&			firmwareFile);
static void usage();


void setup(void) {
	// init radio for reading
	radio.begin();
	radio.setRetries(15, 15);
	radio.setDataRate(RF24_1MBPS);
	radio.setPALevel(RF24_PA_MAX);
	radio.setChannel(76);
	radio.setAutoAck(1);
	radio.setAddressWidth(3);
	radio.setPayloadSize(32);
	radio.setCRCLength(RF24_CRC_8);
	radio.printDetails();

	radio.openWritingPipe(tx_address);
	radio.openReadingPipe(1, rx_address);
	radio.stopListening();

	ifstream is;
	is.open("fw.bin", ios::binary | ios::in);
	if (is.fail()) {
		printf("Error while opening firmware file\r\n");
		return;
	}

	is.seekg(0, ios::end);
	int length = is.tellg();
	is.seekg(0, ios::beg);

	printf("\r\n----------%d----------\r\n", length);

	int hexPerLine = 0;
	int hexLen = 0;

	char *fwBuffer = new char[length];
	// read data as a block:
	is.read(fwBuffer, length);
	is.close();

	printf(
			"\r\n------------------------------------------------------------\r\n");

	hexLen = 0;
	hexPerLine = 0;
	int startIdx = 0;
	int bufLength = 32;
	int buf = bufLength;
	while (startIdx < length) {
		char radioBuffer[bufLength];
		memcpy_index(&radioBuffer, fwBuffer, startIdx, buf);
		startIdx += bufLength;
		if (startIdx + bufLength > length) {
			buf = length - startIdx;
		}

		printf(
				"\r\n------------------start: %d count: %d------------------\r\n",
				startIdx, sizeof(radioBuffer));
		radio.writeBlocking(&radioBuffer, sizeof(radioBuffer), 1000);

		for (unsigned int i = 0; i < sizeof(radioBuffer); i++) {
			hexLen++;
			printf("%02x", (char) radioBuffer[i]);
			if (hexLen == 4) {
				hexLen = 0;
				hexPerLine++;
				printf(" ");
			}
			if (hexPerLine == 14) {
				hexPerLine = 0;
				printf("\r\n");
			}
		}
	}

	if (!radio.txStandBy()) {
		printf("Error while sending data\r\n");
	}

//    while(!is.eof()) {
//    	is.read((char*)&buffer, bufLength);
//    	length -= bufLength;
//    	if (length < 0) {
//    		bufLength = length+1;
//    	}
//    	printf("\r\n----------%d-------\r\n", bufLength);
//
//
////    	if (!radio.write(buffer, bufLength)) {
////    		printf("Error while sending data\r\n");
////    		break;
////    	}
//    }
	printf("\r\n");
	printf("File success sent\r\n");
}

int main(int argc, char** argv) {
	HomeConfigParser * parser;
	const char * homeConfigFile;
	const char * homeDeviceName;
	const char * firmwareFile;

	setlocale(LC_ALL, "");
	parseCmdLineArgs(argc, argv, homeConfigFile, homeDeviceName, firmwareFile);

	parser = new HomeConfigParser();

	try {
		parser->parse(homeConfigFile, "home");
	} catch(const HomeConfigParserException & ex) {
		fprintf(stderr, "%s\n", ex.c_str());
		delete parser;
		return 1;
	}

	FWUpdater *updater = new FWUpdater(parser);

//	cout << "Driver initialized, please check values of /dev/nrf24" << endl;
//	setup();
//    while(1)
//        loop();

	return 0;
}

static void
parseCmdLineArgs(
	int						argc,
	char **					argv,
	const char *&			homeConfigFile,
	const char *&			homeDeviceName,
	const char *&			firmwareFile)
{
	int				i;

	homeConfigFile = "";
	homeDeviceName = "";
	firmwareFile   = "";

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0) {
			usage();
		} else if (strcmp(argv[i], "-device") == 0) {
			if (i == argc-1) { usage(); }
			homeDeviceName = argv[i+1];
			i++;
		} else if (strcmp(argv[i], "-config") == 0) {
			if (i == argc-1) { usage(); }
			homeConfigFile = argv[i+1];
			i++;
		}
	}

	firmwareFile = argv[argc-1];

	if (strcmp(firmwareFile, "") == 0) {
		usage();
	}
}



static void
usage()
{
	fprintf(stderr,
		"\n"
		"usage: fw-updater <options> <firmware.bin>\n"
		"\n"
		"The <options> can be:\n"
		"  -h                Print this usage statement\n"
		"  -device           set device for update\n"
		"  -config           home config file path\n\n");
	exit(1);
}

