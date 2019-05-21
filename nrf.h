#ifndef NRF_H_
#define NRF_H_

#include "RF24/RF24.h"

#define NRF_CMD_BOOTLOAD 0xFFFF
#define NRF_CMD_FWDATA 0xFFF0
#define NRF_CMD_RESET 0xFFF1

class NRF {
public:
	NRF(RF24 *radio);
	virtual ~NRF();

	bool reboot_to_bootloader();
	bool is_bootloader_mode();
	bool reset_to_normal();
private:
	RF24 * radio;
};

#endif
