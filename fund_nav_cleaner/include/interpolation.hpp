#pragma once

#include "nav_types.hpp"
#include <vector>

namespace fund_nav_cleaner {

// Class for interpolation algorithms
class Interpolation {
public:
    // Linear interpolation for missing values
    // Returns the number of values filled
    static int linear_interpolate(std::vector<NavRecord>& records);
    
    // Time-weighted interpolation (considers date gaps)
    static int time_weighted_interpolate(std::vector<NavRecord>& records);
    
    // Forward fill: use the last valid value
    static int forward_fill(std::vector<NavRecord>& records);
    
    // Backward fill: use the next valid value
    static int backward_fill(std::vector<NavRecord>& records);
    
    // Median fill: fill with median of surrounding valid values
    static int median_fill(std::vector<NavRecord>& records, int window_size = 5);
    
private:
    // Helper: Find previous valid record index
    static int find_prev_valid(const std::vector<NavRecord>& records, int start_idx);
    
    // Helper: Find next valid record index
    static int find_next_valid(const std::vector<NavRecord>& records, int start_idx);
    
    // Helper: Get valid values in a window
    static std::vector<double> get_window_values(
        const std::vector<NavRecord>& records, 
        int center_idx, 
        int window_size
    );
    
    // Helper: Calculate median
    static double calculate_median(std::vector<double> values);
};

} // namespace fund_nav_cleaner
