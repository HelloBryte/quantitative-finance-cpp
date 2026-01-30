#include "nav_cleaner.hpp"
#include "csv_handler.hpp"
#include <iostream>
#include <iomanip>

using namespace fund_nav_cleaner;

// Generate sample data with intentional issues
std::vector<NavRecord> generate_sample_data() {
    std::vector<NavRecord> records;
    
    // Fund 1: Normal data with some missing values
    for (int i = 0; i < 10; ++i) {
        NavRecord record;
        record.fund_id = "000001";
        record.date = string_to_date("2024-01-" + std::to_string(i + 1));
        
        if (i == 3 || i == 7) {
            // Missing values
            record.nav = std::nullopt;
        } else {
            record.nav = 1.0 + i * 0.01;
        }
        record.accumulated_nav = 1.0 + i * 0.015;
        records.push_back(record);
    }
    
    // Fund 2: Data with outlier
    for (int i = 0; i < 8; ++i) {
        NavRecord record;
        record.fund_id = "000002";
        record.date = string_to_date("2024-01-" + std::to_string(i + 10));
        
        if (i == 4) {
            // Outlier: sudden 500% jump
            record.nav = 5.5;
        } else {
            record.nav = 1.1 + i * 0.01;
        }
        record.accumulated_nav = 1.1 + i * 0.015;
        records.push_back(record);
    }
    
    return records;
}

void print_records(const std::vector<NavRecord>& records, const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << title << "\n";
    std::cout << std::string(60, '=') << "\n";
    std::cout << std::left << std::setw(10) << "Fund ID"
              << std::setw(12) << "Date"
              << std::setw(12) << "NAV"
              << std::setw(12) << "Accum NAV"
              << "Status\n";
    std::cout << std::string(60, '-') << "\n";
    
    for (const auto& record : records) {
        std::cout << std::left << std::setw(10) << record.fund_id
                  << std::setw(12) << date_to_string(record.date);
        
        if (record.has_value()) {
            std::cout << std::setw(12) << std::fixed << std::setprecision(4) 
                      << record.get_value();
        } else {
            std::cout << std::setw(12) << "N/A";
        }
        
        std::cout << std::setw(12) << std::fixed << std::setprecision(4) 
                  << record.accumulated_nav
                  << validation_status_to_string(record.status) << "\n";
    }
}

void print_result(const CleaningResult& result) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "CLEANING RESULTS\n";
    std::cout << std::string(60, '=') << "\n";
    std::cout << "Total Records:       " << result.total_records << "\n";
    std::cout << "Missing Filled:      " << result.missing_filled << "\n";
    std::cout << "Outliers Detected:   " << result.outliers_detected << "\n";
    std::cout << "Outliers Corrected:  " << result.outliers_corrected << "\n";
    std::cout << "Status:              ";
    
    switch (result.status) {
        case CleaningStatus::SUCCESS:
            std::cout << "SUCCESS\n";
            break;
        case CleaningStatus::NO_CHANGES_NEEDED:
            std::cout << "NO CHANGES NEEDED\n";
            break;
        case CleaningStatus::PARTIALLY_CLEANED:
            std::cout << "PARTIALLY CLEANED\n";
            break;
        case CleaningStatus::FAILED:
            std::cout << "FAILED\n";
            break;
        case CleaningStatus::INSUFFICIENT_DATA:
            std::cout << "INSUFFICIENT DATA\n";
            break;
    }
    
    if (!result.warnings.empty()) {
        std::cout << "\nWarnings:\n";
        for (const auto& warning : result.warnings) {
            std::cout << "  - " << warning << "\n";
        }
    }
    std::cout << std::string(60, '=') << "\n";
}

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║   Fund NAV Data Cleaning Tool Demo                        ║\n";
    std::cout << "║   基金净值数据清洗与修补工具演示                              ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    
    // Generate sample data
    std::cout << "\n[Step 1] Generating sample data with issues...\n";
    auto records = generate_sample_data();
    print_records(records, "ORIGINAL DATA (Before Cleaning)");
    
    // Configure cleaner
    ValidationConfig val_config;
    val_config.max_daily_change_pct = 0.5;  // 50% max daily change
    val_config.z_score_threshold = 3.0;
    
    CleaningConfig clean_config;
    clean_config.default_method = CleaningMethod::LINEAR_INTERPOLATION;
    clean_config.knn_neighbors = 3;
    
    NavCleaner cleaner(val_config, clean_config);
    
    // Scenario 1: Clean all data
    std::cout << "\n[Step 2] Cleaning data with LINEAR INTERPOLATION...\n";
    auto records_copy1 = records;
    CleaningResult result1 = cleaner.clean(records_copy1);
    print_result(result1);
    print_records(result1.cleaned_data, "CLEANED DATA (Linear Interpolation)");
    
    // Scenario 2: Clean with KNN
    std::cout << "\n[Step 3] Cleaning data with KNN IMPUTATION...\n";
    clean_config.default_method = CleaningMethod::KNN_IMPUTATION;
    cleaner.set_cleaning_config(clean_config);
    
    auto records_copy2 = records;
    CleaningResult result2 = cleaner.clean(records_copy2);
    print_result(result2);
    print_records(result2.cleaned_data, "CLEANED DATA (KNN Imputation)");
    
    // Scenario 3: CSV I/O demonstration
    std::cout << "\n[Step 4] Testing CSV file I/O...\n";
    
    // Write original data
    std::string input_file = "sample_nav_data.csv";
    std::string output_file = "cleaned_nav_data.csv";
    std::string report_file = "cleaning_report.txt";
    
    std::cout << "  Writing original data to: " << input_file << "\n";
    CsvHandler::write_csv(input_file, records, true);
    
    // Read and clean
    std::cout << "  Reading data from CSV...\n";
    auto csv_records = CsvHandler::read_csv(input_file);
    std::cout << "  Read " << csv_records.size() << " records\n";
    
    std::cout << "  Cleaning data...\n";
    CleaningResult csv_result = cleaner.clean(csv_records);
    
    // Write cleaned data and report
    std::cout << "  Writing cleaned data to: " << output_file << "\n";
    CsvHandler::write_csv(output_file, csv_result.cleaned_data, true);
    
    std::cout << "  Writing report to: " << report_file << "\n";
    CsvHandler::write_report(report_file, csv_result);
    
    // Scenario 4: Clean multiple funds separately
    std::cout << "\n[Step 5] Cleaning multiple funds separately...\n";
    auto records_copy3 = records;
    auto multi_results = cleaner.clean_multiple_funds(records_copy3);
    
    for (const auto& [fund_id, result] : multi_results) {
        std::cout << "\n  Fund: " << fund_id << "\n";
        std::cout << "    Missing filled: " << result.missing_filled << "\n";
        std::cout << "    Outliers corrected: " << result.outliers_corrected << "\n";
    }
    
    std::cout << "\n╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║   Demo completed successfully!                             ║\n";
    std::cout << "║   演示完成！                                                 ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "Generated files:\n";
    std::cout << "  - " << input_file << " (original data)\n";
    std::cout << "  - " << output_file << " (cleaned data)\n";
    std::cout << "  - " << report_file << " (cleaning report)\n\n";
    
    return 0;
}
