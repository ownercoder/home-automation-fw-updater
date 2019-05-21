#include "nrf.h"

NRF::NRF(RF24 *radio)
{
	radio->begin();
	radio->setRetries(15, 15);
	radio->setDataRate(RF24_1MBPS);
	radio->setPALevel(RF24_PA_MAX);
	radio->setChannel(76);
	radio->setAutoAck(1);
	radio->setAddressWidth(3);
	radio->setPayloadSize(32);
	radio->setCRCLength(RF24_CRC_8);

	this->radio = radio;
}

NRF::~NRF()
{

}

bool
NRF::reboot_to_bootloader()
{
return false;
}

bool
NRF::is_bootloader_mode()
{
return false;
}

bool
NRF::reset_to_normal()
{
return false;
}
