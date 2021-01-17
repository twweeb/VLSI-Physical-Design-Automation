#include "Net.hpp"

void Net::updateHPWL()
{
    int minX{INF}, minY{INF}, maxX{0}, maxY{0};
    for (const auto& coord: pins_coord_)
    {
        minX = std::min(coord.first, minX);
        minY = std::min(coord.second, minY);
        maxX = std::max(coord.first, maxX);
        maxY = std::max(coord.second, maxY);
    }

    HPWL_ = (maxX - minX) + (maxY - minY);
}