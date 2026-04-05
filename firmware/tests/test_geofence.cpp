#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "geofence.hpp"

TEST_CASE("Haversine distance calculation", "[geofence]") {
    SECTION("Same point returns zero") {
        int32_t lat = 40000000;
        int32_t lon = -74000000;
        REQUIRE(haversine_distance(lat, lon, lat, lon) == 0);
    }

    SECTION("Known distance NYC to LA is approximately 3940km") {
        int32_t nyc_lat = 40760000;
        int32_t nyc_lon = -73940000;
        int32_t la_lat = 34030000;
        int32_t la_lon = -118150000;
        int64_t dist = haversine_distance(nyc_lat, nyc_lon, la_lat, la_lon);
        REQUIRE(dist > 3900000);
        REQUIRE(dist < 4000000);
    }

    SECTION("Equator points 1 degree apart is approximately 111km") {
        int32_t lat = 0;
        int32_t lon1 = 0;
        int32_t lon2 = 1000000;
        int64_t dist = haversine_distance(lat, lon1, lat, lon2);
        REQUIRE(dist > 110000);
        REQUIRE(dist < 112000);
    }

    SECTION("Antimeridian crossing - Tokyo to San Francisco") {
        int32_t tokyo_lat = 35652000;
        int32_t tokyo_lon = 139745000;
        int32_t sf_lat = 37774700;
        int32_t sf_lon = -122420000;
        int64_t dist = haversine_distance(tokyo_lat, tokyo_lon, sf_lat, sf_lon);
        REQUIRE(dist > 8200000);
        REQUIRE(dist < 8400000);
    }

    SECTION("North pole to equator") {
        int32_t pole_lat = 90000000;
        int32_t eq_lat = 0;
        int32_t lon = 0;
        int64_t dist = haversine_distance(pole_lat, lon, eq_lat, lon);
        REQUIRE(dist > 10000000);
        REQUIRE(dist < 10050000);
    }
}

TEST_CASE("Coordinate validation", "[geofence]") {
    SECTION("Valid coordinates pass") {
        REQUIRE(coordinates_valid(40000000, -74000000) == true);
        REQUIRE(coordinates_valid(0, 0) == true);
        REQUIRE(coordinates_valid(-60000000, 180000000) == true);
    }

    SECTION("Invalid latitude fails") {
        REQUIRE(coordinates_valid(95000000, 0) == false);
        REQUIRE(coordinates_valid(-95000000, 0) == false);
    }

    SECTION("Invalid longitude fails") {
        REQUIRE(coordinates_valid(0, 185000000) == false);
        REQUIRE(coordinates_valid(0, -185000000) == false);
    }
}

TEST_CASE("Zone name safety", "[geofence]") {
    SECTION("Short name fits") {
        CircleZone zone = {{0}};
        zone_set_name(zone, "Home");
        REQUIRE(strcmp(zone.name, "Home") == 0);
    }

    SECTION("Long name truncated safely") {
        CircleZone zone = {{0}};
        zone_set_name(zone, "This is a very long zone name that exceeds limit");
        REQUIRE(strlen(zone.name) < 32);
        REQUIRE(zone.name[30] == '\0');
    }
}

TEST_CASE("Point in circle detection", "[geofence]") {
    CircleZone zone = {
        .name = "TestZone",
        .center = {40000000, -74000000},
        .radius_m = 1000
    };

    SECTION("Point at center is inside") {
        GeoPoint point = {40000000, -74000000};
        REQUIRE(point_in_circle(point, zone) == true);
    }

    SECTION("Point well within radius is inside") {
        GeoPoint point = {40000500, -74000500};
        REQUIRE(point_in_circle(point, zone) == true);
    }

    SECTION("Point outside radius is outside") {
        GeoPoint point = {40010000, -74010000};
        REQUIRE(point_in_circle(point, zone) == false);
    }

    SECTION("Invalid point coordinates returns false") {
        GeoPoint bad_point = {95000000, 0};
        REQUIRE(point_in_circle(bad_point, zone) == false);
    }

    SECTION("Very small radius zone") {
        CircleZone tiny_zone = {
            .name = "TinyZone",
            .center = {40000000, -74000000},
            .radius_m = 1
        };
        GeoPoint center_point = {40000000, -74000000};
        GeoPoint near_point = {40000001, -74000000};
        GeoPoint far_point = {40000100, -74000000};
        REQUIRE(point_in_circle(center_point, tiny_zone) == true);
        REQUIRE(point_in_circle(near_point, tiny_zone) == true);
        REQUIRE(point_in_circle(far_point, tiny_zone) == false);
    }
}
