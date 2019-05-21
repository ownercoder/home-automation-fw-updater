/*
 * FWUpdater.h
 *
 *  Created on: 20 мая 2019 г.
 *      Author: Sergey Kvartnikov <me@kvartnikov.info>
 */

#ifndef FWUPDATER_H_
#define FWUPDATER_H_

#include "RF24/RF24.h"
#include "HomeConfigParser.h"

class FWUpdater {
public:
	FWUpdater(HomeConfigParser *config);
	virtual ~FWUpdater();
private:
	RF24 * radio;
};

#endif /* FWUPDATER_H_ */
