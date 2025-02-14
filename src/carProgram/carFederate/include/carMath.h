#ifndef CARMATH_H
#define CARMATH_H

#include <cmath>
#include <iostream>

// Function to convert degrees to radians
double toRadians(double degree) {
    return degree * (M_PI / 180.0);
}

// Function to calculate the Haversine distance
double haversineDistance(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371.0;
    
    lat1 = toRadians(lat1);
    lon1 = toRadians(lon1);
    lat2 = toRadians(lat2);
    lon2 = toRadians(lon2);

    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    double a = sin(dlat / 2) * sin(dlat / 2) +
               cos(lat1) * cos(lat2) * sin(dlon / 2) * sin(dlon / 2);

    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return R * c;
}

#endif 