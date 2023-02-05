#include "domain.h"

/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области
 * (domain) вашего приложения и не зависят от транспортного справочника. Например Автобусные
 * маршруты и Остановки.
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */
size_t Bus::route_size() const {
    return route_.size();
}

bool Bus::is_ring() const {
    return is_ring_;
}
/*
std::set<Stop*, stop_compare> stops_in_alphabetical(const std::set<Bus*, bus_compare>& buses) {
    std::set<Stop*, stop_compare> stops_in_alphabetical_;
    for(const auto bus : buses) {
        for(const auto stop : bus->route_) {
            stops_in_alphabetical_.insert(stop);
        }
    }
    return stops_in_alphabetical_;
}*/
