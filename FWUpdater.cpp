/*
 * FWUpdater.cpp
 *
 *  Created on: 20 мая 2019 г.
 *      Author: sergey
 */

#include "FWUpdater.h"

FWUpdater::FWUpdater(HomeConfigParser *config) {
	NRF_Config nrf = config->getNRFConfig();
	radio = new RF24(nrf.ce_pin, 0);
}

FWUpdater::~FWUpdater() {
	// TODO Auto-generated destructor stub
}

