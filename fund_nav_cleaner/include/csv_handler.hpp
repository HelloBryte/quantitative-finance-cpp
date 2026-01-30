#pragma once

#include "nav_types.hpp"
#include <string>
#include <vector>
#include <fstream>

namespace fund_nav_cleaner {

// Class for handling CSV file I/O
class CsvHandler {
public:
    // Read NAV records from CSV file
    // Expected format: fund_id,date,nav,accumulated_nav
    static std::vector<NavRecord> read_csv(const std::string& filename);
    
    // Write NAV records to CSV file
    static bool write_csv(
        const std::string& filename,
        const std::vector<NavRecord>& records,
        bool include_status = false
    );
    
    // Write cleaning result report
    static bool write_report(
        const std::string& filename,
        const CleaningResult& result
    );
    
    // Append records to existing CSV
    static bool append_csv(
        const std::string& filename,
        const std::vector<NavRecord>& records
    );
    
private:
    // Helper: Parse a single CSV line
    static NavRecord parse_line(const std::string& line);
    
    // Helper: Convert record to CSV line
    static std::string to_csv_line(const NavRecord& record, bool include_status);
    
    // Helper: Split string by delimiter
    static std::vector<std::string> split(const std::string& str, char delimiter);
    
    // Helper: Trim whitespace
    static std::string trim(const std::string& str);
    
    // Helper: Validate CSV header
    static bool validate_header(const std::string& header);
};

} // namespace fund_nav_cleaner
