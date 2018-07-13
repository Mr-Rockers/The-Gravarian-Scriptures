#ifndef MATHHELPER_H
#define MATHHELPER_H

#include "Includes.h"

//Applies the angles to be within bounds of 0 and 2 PI.
void WrapAngles(double* angleIn) {

	//unsigned int order = (int)(*angleIn * M_PI / 180.0) % 360;

	if (*angleIn < 0.0) {
		*angleIn += M_PI * 2.0; // *(order <= 0 ? 1.0 : (double)order);
	}
	else if (*angleIn > M_PI * 2.0) {
		*angleIn -= M_PI * 2.0; // *(order <= 0 ? 1.0 : (double)order);
	}
}

//Applies the angles to be within bounds of 0 and 360.
void WrapAnglesDegrees(double* angleIn) {

	//unsigned int order = (int)(*angleIn) % 360;

	if (*angleIn < 0.0) {
		*angleIn += 360.0; // *(order <= 0 ? 1.0 : (double)order);
	}
	else if (*angleIn > 360.0) {
		*angleIn -= 360.0; // *(order <= 0 ? 1.0 : (double)order);
	}
}

#endif