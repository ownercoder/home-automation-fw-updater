#include <cstdlib>
#include <iostream>
#include "RF24/RF24.h"
#include "FWUpdater.h"
#include "nrf.h"
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

	int hexLen = 0;

	char *fwBuffer = new char[length];
	// read data as a block:
	is.read(fwBuffer, length);
	is.close();

	printf(
			"\r\n------------------------------------------------------------\r\n");

	hexLen = 0;
	int startIdx = 0;
	int bufLength = 30;
	uint16_t operationByte = NRF_CMD_FWDATA;
	int buf = bufLength;
	char *data = new char[32];

	while (startIdx < length) {
		memset(data, 1, sizeof(*data));
		memcpy(data, (const void*)&operationByte, sizeof(operationByte));

		memcpy_index(data + sizeof(operationByte), fwBuffer, startIdx, buf);
		startIdx += bufLength;
		if (startIdx + bufLength > length) {
			buf = length - startIdx;
		}

		printf(
				"\r\n------------------start: %d count: %d------------------\r\n",
				startIdx, sizeof(*data));

		radio.writeBlocking(data, sizeof(*data), 1000);

		for (unsigned int i = 0; i < sizeof(*data); i++) {
			hexLen++;
			printf("%02X", data[i]);
			if (hexLen == 4) {
				hexLen = 0;
				printf(" ");
			}
		}
	}

	if (!radio.txStandBy()) {
		printf("Error while sending data\r\n");
	}

	operationByte = NRF_CMD_RESET;
	memset(data, 1, sizeof(*data));
	memcpy(data, (const void*)&operationByte, sizeof(operationByte));
	radio.writeBlocking(data, sizeof(*data), 1000);

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

