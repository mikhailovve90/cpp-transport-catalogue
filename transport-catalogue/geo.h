#pragma once

namespace geo {

const long earth_radius = 6371000;

struct Coordinates {
    double lat; // Широта
    double lng; // Долгота
    bool operator==(const Coordinates& other) const {
        return lat == other.lat && lng == other.lng;
    }
    bool operator!=(const Coordinates& other) const {
        return !(*this == other);
    }
};

double compute_distance(Coordinates from, Coordinates to);

}  // namespace geo
