#include "csv_handler.hpp"
#include <sstream>
#include <algorithm>
#include <iostream>

namespace fund_nav_cleaner {

std::vector<NavRecord> CsvHandler::read_csv(const std::string& filename) {
    std::vector<NavRecord> records;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return records;
    }
    
    std::string line;
    bool first_line = true;
    
    while (std::getline(file, line)) {
        // Skip header line
        if (first_line) {
            if (!validate_header(line)) {
                std::cerr << "Warning: Invalid CSV header format" << std::endl;
            }
            first_line = false;
            continue;
        }
        
        // Skip empty lines
        if (line.empty()) {
            continue;
        }
        
        try {
            NavRecord record = parse_line(line);
            records.push_back(record);
        } catch (const std::exception& e) {
            std::cerr << "Error parsing line: " << line << " - " << e.what() << std::endl;
        }
    }
    
    file.close();
    return records;
}

bool CsvHandler::write_csv(
    const std::string& filename,
    const std::vector<NavRecord>& records,
    bool include_status) {
    
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing" << std::endl;
        return false;
    }
    
    // Write header
    if (include_status) {
        file << "fund_id,date,nav,accumulated_nav,status\n";
    } else {
        file << "fund_id,date,nav,accumulated_nav\n";
    }
    
    // Write records
    for (const auto& record : records) {
        file << to_csv_line(record, include_status) << "\n";
    }
    
    file.close();
    return true;
}

bool CsvHandler::write_report(
    const std::string& filename,
    const CleaningResult& result) {
    
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing" << std::endl;
        return false;
    }
    
    // Write summary
    file << "=== Fund NAV Cleaning Report ===\n\n";
    file << "Total Records: " << result.total_records << "\n";
    file << "Missing Values Filled: " << result.missing_filled << "\n";
    file << "Outliers Detected: " << result.outliers_detected << "\n";
    file << "Outliers Corrected: " << result.outliers_corrected << "\n";
    file << "Status: ";
    
    switch (result.status) {
        case CleaningStatus::SUCCESS:
            file << "SUCCESS\n";
            break;
        case CleaningStatus::NO_CHANGES_NEEDED:
            file << "NO_CHANGES_NEEDED\n";
            break;
        case CleaningStatus::PARTIALLY_CLEANED:
            file << "PARTIALLY_CLEANED\n";
            break;
        case CleaningStatus::FAILED:
            file << "FAILED\n";
            break;
        case CleaningStatus::INSUFFICIENT_DATA:
            file << "INSUFFICIENT_DATA\n";
            break;
    }
    
    // Write warnings
    if (!result.warnings.empty()) {
        file << "\nWarnings:\n";
        for (const auto& warning : result.warnings) {
            file << "  - " << warning << "\n";
        }
    }
    
    file.close();
    return true;
}

bool CsvHandler::append_csv(
    const std::string& filename,
    const std::vector<NavRecord>& records) {
    
    std::ofstream file(filename, std::ios::app);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for appending" << std::endl;
        return false;
    }
    
    for (const auto& record : records) {
        file << to_csv_line(record, false) << "\n";
    }
    
    file.close();
    return true;
}

NavRecord CsvHandler::parse_line(const std::string& line) {
    NavRecord record;
    
    std::vector<std::string> fields = split(line, ',');
    
    if (fields.size() < 4) {
        throw std::runtime_error("Invalid CSV line format: insufficient fields");
    }
    
    // Parse fund_id
    record.fund_id = trim(fields[0]);
    
    // Parse date
    try {
        record.date = string_to_date(trim(fields[1]));
    } catch (...) {
        throw std::runtime_error("Invalid date format");
    }
    
    // Parse nav
    std::string nav_str = trim(fields[2]);
    if (nav_str.empty() || nav_str == "NA" || nav_str == "N/A" || nav_str == "null") {
        record.nav = std::nullopt;
    } else {
        try {
            record.nav = std::stod(nav_str);
        } catch (...) {
            record.nav = std::nullopt;
        }
    }
    
    // Parse accumulated_nav
    try {
        record.accumulated_nav = std::stod(trim(fields[3]));
    } catch (...) {
        record.accumulated_nav = 0.0;
    }
    
    return record;
}

std::string CsvHandler::to_csv_line(const NavRecord& record, bool include_status) {
    std::ostringstream oss;
    
    oss << record.fund_id << ",";
    oss << date_to_string(record.date) << ",";
    
    if (record.has_value()) {
        oss << record.get_value();
    } else {
        oss << "NA";
    }
    
    oss << "," << record.accumulated_nav;
    
    if (include_status) {
        oss << "," << validation_status_to_string(record.status);
    }
    
    return oss.str();
}

std::vector<std::string> CsvHandler::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream token_stream(str);
    
    while (std::getline(token_stream, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string CsvHandler::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

bool CsvHandler::validate_header(const std::string& header) {
    std::string lower_header = header;
    std::transform(lower_header.begin(), lower_header.end(), lower_header.begin(), ::tolower);
    
    return lower_header.find("fund_id") != std::string::npos &&
           lower_header.find("date") != std::string::npos &&
           lower_header.find("nav") != std::string::npos;
}

} // namespace fund_nav_cleaner
