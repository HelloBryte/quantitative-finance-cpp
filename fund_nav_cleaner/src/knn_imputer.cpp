#include "knn_imputer.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace fund_nav_cleaner {

KnnImputer::KnnImputer(int k) : k_(k) {
    if (k < 1) {
        throw std::invalid_argument("K must be at least 1");
    }
}

void KnnImputer::set_k(int k) {
    if (k < 1) {
        throw std::invalid_argument("K must be at least 1");
    }
    k_ = k;
}

int KnnImputer::impute(std::vector<NavRecord>& records) {
    int filled_count = 0;
    
    if (records.size() < static_cast<size_t>(k_ + 1)) {
        return 0;  // Not enough data for KNN
    }
    
    // Find and fill missing values
    for (size_t i = 0; i < records.size(); ++i) {
        if (!records[i].has_value()) {
            std::vector<Neighbor> neighbors = find_neighbors(i, records);
            
            if (!neighbors.empty()) {
                double imputed_value = calculate_weighted_average(neighbors, records);
                records[i].nav = imputed_value;
                filled_count++;
            }
        }
    }
    
    return filled_count;
}

double KnnImputer::calculate_distance(
    const NavRecord& record1,
    const NavRecord& record2,
    const std::vector<NavRecord>& all_records) const {
    
    // Distance based on temporal similarity
    double temporal_dist = get_temporal_similarity(record1.date, record2.date);
    
    // Normalize: closer dates have smaller distance
    // Scale by a factor to make temporal difference significant
    return temporal_dist;
}

std::vector<KnnImputer::Neighbor> KnnImputer::find_neighbors(
    int target_idx,
    const std::vector<NavRecord>& records) const {
    
    std::vector<Neighbor> all_neighbors;
    const NavRecord& target = records[target_idx];
    
    // Find all valid records and calculate distances
    for (size_t i = 0; i < records.size(); ++i) {
        if (i != static_cast<size_t>(target_idx) && records[i].has_value()) {
            double dist = calculate_distance(target, records[i], records);
            all_neighbors.push_back({static_cast<int>(i), dist});
        }
    }
    
    // Sort by distance (ascending)
    std::sort(all_neighbors.begin(), all_neighbors.end());
    
    // Return top K neighbors
    int actual_k = std::min(k_, static_cast<int>(all_neighbors.size()));
    return std::vector<Neighbor>(all_neighbors.begin(), all_neighbors.begin() + actual_k);
}

double KnnImputer::calculate_weighted_average(
    const std::vector<Neighbor>& neighbors,
    const std::vector<NavRecord>& records) const {
    
    if (neighbors.empty()) {
        return 0.0;
    }
    
    double weighted_sum = 0.0;
    double weight_sum = 0.0;
    
    for (const auto& neighbor : neighbors) {
        // Use inverse distance as weight (closer neighbors have more influence)
        // Add small epsilon to avoid division by zero
        double weight = 1.0 / (neighbor.distance + 1.0);
        
        weighted_sum += records[neighbor.index].get_value() * weight;
        weight_sum += weight;
    }
    
    return weight_sum > 0 ? weighted_sum / weight_sum : 0.0;
}

double KnnImputer::get_temporal_similarity(const std::tm& date1, const std::tm& date2) const {
    // Calculate absolute difference in days
    int day_diff = days_between(date1, date2);
    
    // Also consider day of week/month similarity (cyclical patterns)
    int day_of_week_diff = std::abs(date1.tm_wday - date2.tm_wday);
    int day_of_month_diff = std::abs(date1.tm_mday - date2.tm_mday);
    
    // Weighted combination
    double distance = static_cast<double>(day_diff) + 
                     0.1 * day_of_week_diff + 
                     0.05 * day_of_month_diff;
    
    return distance;
}

} // namespace fund_nav_cleaner
