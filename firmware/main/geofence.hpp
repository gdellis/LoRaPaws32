#pragma once

#include <stdint.h>
#include <stdbool.h>

constexpr int32_t LATLON_MULTIPLIER = 1000000;

struct GeoPoint {
    int32_t latitude;   // degrees * 1e6
    int32_t longitude;   // degrees * 1e6
};

struct CircleZone {
    char name[32];
    GeoPoint center;
    uint32_t radius_m;   // meters
};

bool point_in_circle(const GeoPoint& point, const CircleZone& zone);

int64_t haversine_distance(int32_t lat1, int32_t lon1, int32_t lat2, int32_t lon2);
