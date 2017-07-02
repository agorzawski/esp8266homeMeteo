#ifndef METEO_UTILS_H_
#define METEO_UTILS_H_

float getNormalizedPressure(float pressure, float altitudeInMeters)
{
  return pressure / exp( -9.81 * 0.0289644 * altitudeInMeters / ( 8.31447 * 288.15) );
}

#endif /* METEO_UTILS_H_ */
