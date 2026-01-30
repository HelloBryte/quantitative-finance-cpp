#pragma once

#include "nav_types.hpp"
#include <vector>

namespace fund_nav_cleaner {

// Class for K-Nearest Neighbors imputation
class KnnImputer {
public:
    explicit KnnImputer(int k = 5);
    
    // Impute missing values using KNN algorithm
    // Based on historical patterns from similar dates
    int impute(std::vector<NavRecord>& records);
    
    // Get/Set K value
    int get_k() const { return k_; }
    void set_k(int k);
    
private:
    int k_;  // Number of neighbors
    
    // Structure to hold distance and index
    struct Neighbor {
        int index;
        double distance;
        
        bool operator<(const Neighbor& other) const {
            return distance < other.distance;
        }
    };
    
    // Calculate distance between two records
    // Uses temporal distance and NAV similarity
    double calculate_distance(
        const NavRecord& record1,
        const NavRecord& record2,
        const std::vector<NavRecord>& all_records
    ) const;
    
    // Find K nearest neighbors for a missing value
    std::vector<Neighbor> find_neighbors(
        int target_idx,
        const std::vector<NavRecord>& records
    ) const;
    
    // Calculate weighted average from neighbors
    double calculate_weighted_average(
        const std::vector<Neighbor>& neighbors,
        const std::vector<NavRecord>& records
    ) const;
    
    // Get temporal features for distance calculation
    double get_temporal_similarity(const std::tm& date1, const std::tm& date2) const;
};

} // namespace fund_nav_cleaner
