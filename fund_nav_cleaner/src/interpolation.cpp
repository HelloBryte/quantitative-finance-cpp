#include "interpolation.hpp"
#include <algorithm>
#include <numeric>

namespace fund_nav_cleaner {

int Interpolation::linear_interpolate(std::vector<NavRecord>& records) {
    int filled_count = 0;
    
    if (records.size() < 2) {
        return 0;
    }
    
    for (size_t i = 0; i < records.size(); ++i) {
        if (!records[i].has_value()) {
            int prev_idx = find_prev_valid(records, i);
            int next_idx = find_next_valid(records, i);
            
            if (prev_idx >= 0 && next_idx >= 0) {
                double prev_nav = records[prev_idx].get_value();
                double next_nav = records[next_idx].get_value();
                int total_gap = next_idx - prev_idx;
                int current_gap = i - prev_idx;
                
                // Linear interpolation
                double interpolated = prev_nav + 
                    (next_nav - prev_nav) * current_gap / total_gap;
                
                records[i].nav = interpolated;
                filled_count++;
            }
        }
    }
    
    return filled_count;
}

int Interpolation::time_weighted_interpolate(std::vector<NavRecord>& records) {
    int filled_count = 0;
    
    if (records.size() < 2) {
        return 0;
    }
    
    for (size_t i = 0; i < records.size(); ++i) {
        if (!records[i].has_value()) {
            int prev_idx = find_prev_valid(records, i);
            int next_idx = find_next_valid(records, i);
            
            if (prev_idx >= 0 && next_idx >= 0) {
                double prev_nav = records[prev_idx].get_value();
                double next_nav = records[next_idx].get_value();
                
                // Calculate actual days between dates
                int days_to_prev = days_between(records[prev_idx].date, records[i].date);
                int days_to_next = days_between(records[i].date, records[next_idx].date);
                int total_days = days_to_prev + days_to_next;
                
                if (total_days > 0) {
                    // Time-weighted interpolation
                    double interpolated = prev_nav + 
                        (next_nav - prev_nav) * days_to_prev / total_days;
                    
                    records[i].nav = interpolated;
                    filled_count++;
                }
            }
        }
    }
    
    return filled_count;
}

int Interpolation::forward_fill(std::vector<NavRecord>& records) {
    int filled_count = 0;
    
    double last_valid = 0.0;
    bool has_valid = false;
    
    for (auto& record : records) {
        if (record.has_value()) {
            last_valid = record.get_value();
            has_valid = true;
        } else if (has_valid) {
            record.nav = last_valid;
            filled_count++;
        }
    }
    
    return filled_count;
}

int Interpolation::backward_fill(std::vector<NavRecord>& records) {
    int filled_count = 0;
    
    double next_valid = 0.0;
    bool has_valid = false;
    
    // Traverse backwards
    for (int i = records.size() - 1; i >= 0; --i) {
        if (records[i].has_value()) {
            next_valid = records[i].get_value();
            has_valid = true;
        } else if (has_valid) {
            records[i].nav = next_valid;
            filled_count++;
        }
    }
    
    return filled_count;
}

int Interpolation::median_fill(std::vector<NavRecord>& records, int window_size) {
    int filled_count = 0;
    
    if (records.empty() || window_size < 1) {
        return 0;
    }
    
    for (size_t i = 0; i < records.size(); ++i) {
        if (!records[i].has_value()) {
            std::vector<double> window_values = get_window_values(records, i, window_size);
            
            if (!window_values.empty()) {
                double median = calculate_median(window_values);
                records[i].nav = median;
                filled_count++;
            }
        }
    }
    
    return filled_count;
}

int Interpolation::find_prev_valid(const std::vector<NavRecord>& records, int start_idx) {
    for (int i = start_idx - 1; i >= 0; --i) {
        if (records[i].has_value()) {
            return i;
        }
    }
    return -1;
}

int Interpolation::find_next_valid(const std::vector<NavRecord>& records, int start_idx) {
    for (size_t i = start_idx + 1; i < records.size(); ++i) {
        if (records[i].has_value()) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

std::vector<double> Interpolation::get_window_values(
    const std::vector<NavRecord>& records,
    int center_idx,
    int window_size) {
    
    std::vector<double> values;
    
    int half_window = window_size / 2;
    int start = std::max(0, center_idx - half_window);
    int end = std::min(static_cast<int>(records.size()), center_idx + half_window + 1);
    
    for (int i = start; i < end; ++i) {
        if (i != center_idx && records[i].has_value()) {
            values.push_back(records[i].get_value());
        }
    }
    
    return values;
}

double Interpolation::calculate_median(std::vector<double> values) {
    if (values.empty()) {
        return 0.0;
    }
    
    std::sort(values.begin(), values.end());
    size_t n = values.size();
    
    if (n % 2 == 0) {
        return (values[n/2 - 1] + values[n/2]) / 2.0;
    } else {
        return values[n/2];
    }
}

} // namespace fund_nav_cleaner
