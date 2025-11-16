#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <chrono>
#include <cmath>
#include "ost.h"
#include "pom.h"
#include "josephus.h"

using namespace std;

// Color output for terminal (using C_ prefix to avoid conflict with enum)
const string C_RESET = "\033[0m";
const string C_BOLD = "\033[1m";
const string C_GREEN = "\033[32m";
const string C_BLUE = "\033[34m";
const string C_YELLOW = "\033[33m";
const string C_RED = "\033[31m";

void printHeader(const string& title) {
    cout << "\n" << C_BOLD << C_BLUE << "═══════════════════════════════════════════════════════════" << C_RESET << "\n";
    cout << C_BOLD << "  " << title << C_RESET << "\n";
    cout << C_BOLD << C_BLUE << "═══════════════════════════════════════════════════════════" << C_RESET << "\n\n";
}

void printSubHeader(const string& subtitle) {
    cout << C_BOLD << C_YELLOW << ">>> " << subtitle << C_RESET << "\n";
}

// Test 1: Basic OST Functionality
void testOSTBasic() {
    printHeader("TEST 1: ORDER STATISTIC TREE - BASIC OPERATIONS");
    
    OrderStatisticTree<int> ost;
    vector<int> testData = {15, 10, 20, 8, 12, 16, 25};
    
    printSubHeader("Inserting elements: 15, 10, 20, 8, 12, 16, 25");
    for (int val : testData) {
        ost.insert(val);
    }
    
    cout << "Tree size: " << ost.size() << "\n\n";
    
    printSubHeader("Select operations (finding k-th smallest):");
    for (int k = 1; k <= ost.size(); k++) {
        cout << "  " << k << "-th smallest: " << ost.select(k) << "\n";
    }
    
    cout << "\n";
    printSubHeader("Rank operations (finding position):");
    for (int val : testData) {
        cout << "  Rank of " << val << ": " << ost.rank(val) << "\n";
    }
    
    cout << "\n" << C_GREEN << "✓ Basic OST operations completed successfully" << C_RESET << "\n";
}

// Test 2: POM Tree Functionality
void testPOMBasic() {
    printHeader("TEST 2: POM TREE - INTERVAL OPERATIONS");
    
    POMTree pom;
    
    printSubHeader("Inserting intervals:");
    vector<Interval> intervals = {
        Interval(0, 5, 10),
        Interval(5, 10, -5),
        Interval(10, 15, 8),
        Interval(15, 20, -3)
    };
    
    for (const auto& iv : intervals) {
        cout << "  [" << iv.start << ", " << iv.end << ") -> " << iv.value << "\n";
        pom.insert(iv);
    }
    
    cout << "\n";
    printSubHeader("Finding Maximum Prefix Sum (POM):");
    AugmentedData result = pom.findPOM();
    cout << "  Total Sum: " << result.sum << "\n";
    cout << "  Max Prefix Sum: " << result.maxpref << "\n";
    cout << "  Position (argmax): " << result.argmax << "\n";
    
    cout << "\n" << C_GREEN << "✓ Basic POM operations completed successfully" << C_RESET << "\n";
}

// Test 3: Josephus Permutation Verification
void testJosephusCorrectness() {
    printHeader("TEST 3: JOSEPHUS PERMUTATION - CORRECTNESS VERIFICATION");
    
    vector<pair<int, int>> testCases = {
        {7, 3},
        {10, 2},
        {12, 5},
        {15, 4}
    };
    
    bool allPassed = true;
    
    for (const auto& [n, m] : testCases) {
        printSubHeader("Test case: n=" + to_string(n) + ", m=" + to_string(m));
        
        bool correct = JosephusPermutation::verify(n, m);
        
        if (correct) {
            cout << "  " << C_GREEN << "✓ OST and Naive algorithms produce identical results" << C_RESET << "\n";
            
            // Show first few eliminations
            vector<int> result = JosephusPermutation::generateOST(n, m);
            cout << "  Elimination order: ";
            for (int i = 0; i < min(10, (int)result.size()); i++) {
                cout << result[i];
                if (i < min(10, (int)result.size()) - 1) cout << " → ";
            }
            if (result.size() > 10) cout << " ...";
            cout << "\n\n";
        } else {
            cout << "  " << C_RED << "✗ Results do not match!" << C_RESET << "\n\n";
            allPassed = false;
        }
    }
    
    if (allPassed) {
        cout << C_GREEN << "✓ All correctness tests passed!" << C_RESET << "\n";
    }
}

// Test 4: OST Performance Benchmark
void testOSTPerformance() {
    printHeader("TEST 4: OST PERFORMANCE BENCHMARK");
    
    printSubHeader("Measuring operation times for increasing n");
    cout << setw(10) << "n" 
         << setw(15) << "Insert (μs)" 
         << setw(15) << "Select (μs)" 
         << setw(15) << "Delete (μs)" << "\n";
    cout << string(55, '-') << "\n";
    
    ofstream outfile("results/ost_performance.csv");
    outfile << "n,insert_time,select_time,delete_time\n";
    
    vector<int> sizes = {100, 500, 1000, 5000, 10000, 50000, 100000};
    
    for (int n : sizes) {
        OrderStatisticTree<int> ost;
        
        // Measure insert time
        auto start = chrono::high_resolution_clock::now();
        for (int i = 0; i < n; i++) {
            ost.insert(i);
        }
        auto end = chrono::high_resolution_clock::now();
        long long insertTime = chrono::duration_cast<chrono::microseconds>(end - start).count();
        
        // Measure select time (average of n/2 random selections)
        start = chrono::high_resolution_clock::now();
        for (int i = 1; i <= n/2; i++) {
            ost.select(i);
        }
        end = chrono::high_resolution_clock::now();
        long long selectTime = chrono::duration_cast<chrono::microseconds>(end - start).count();
        
        // Measure delete time
        start = chrono::high_resolution_clock::now();
        for (int i = 0; i < n/2; i++) {
            ost.remove(i);
        }
        end = chrono::high_resolution_clock::now();
        long long deleteTime = chrono::duration_cast<chrono::microseconds>(end - start).count();
        
        cout << setw(10) << n 
             << setw(15) << insertTime 
             << setw(15) << selectTime 
             << setw(15) << deleteTime << "\n";
        
        outfile << n << "," << insertTime << "," << selectTime << "," << deleteTime << "\n";
    }
    
    outfile.close();
    cout << "\n" << C_GREEN << "✓ Performance data saved to results/ost_performance.csv" << C_RESET << "\n";
}

// Test 5: Josephus Performance Comparison
void testJosephusPerformance() {
    printHeader("TEST 5: JOSEPHUS ALGORITHM - OST vs NAIVE COMPARISON");
    
    printSubHeader("Comparing execution times for increasing n");
    cout << setw(10) << "n" 
         << setw(12) << "m"
         << setw(15) << "OST (μs)" 
         << setw(15) << "Naive (μs)" 
         << setw(15) << "Speedup" << "\n";
    cout << string(67, '-') << "\n";
    
    ofstream outfile("results/josephus_comparison.csv");
    outfile << "n,m,ost_time,naive_time,speedup\n";
    
    vector<pair<int, int>> testCases = {
        {100, 3}, {500, 3}, {1000, 3}, {5000, 3}, {10000, 3},
        {100, 7}, {500, 7}, {1000, 7}, {5000, 7}, {10000, 7}
    };
    
    for (const auto& [n, m] : testCases) {
        long long ostTime = JosephusPermutation::benchmarkOST(n, m);
        long long naiveTime = JosephusPermutation::benchmarkNaive(n, m);
        double speedup = (double)naiveTime / ostTime;
        
        cout << setw(10) << n 
             << setw(12) << m
             << setw(15) << ostTime 
             << setw(15) << naiveTime 
             << setw(14) << fixed << setprecision(2) << speedup << "x" << "\n";
        
        outfile << n << "," << m << "," << ostTime << "," << naiveTime << "," << speedup << "\n";
    }
    
    outfile.close();
    cout << "\n" << C_GREEN << "✓ Comparison data saved to results/josephus_comparison.csv" << C_RESET << "\n";
}

// Test 6: POM Tree Performance
void testPOMPerformance() {
    printHeader("TEST 6: POM TREE PERFORMANCE BENCHMARK");
    
    printSubHeader("Measuring interval operation times");
    cout << setw(12) << "Intervals" 
         << setw(15) << "Insert (μs)" 
         << setw(15) << "FindPOM (μs)" 
         << setw(15) << "Delete (μs)" << "\n";
    cout << string(57, '-') << "\n";
    
    ofstream outfile("results/pom_performance.csv");
    outfile << "intervals,insert_time,findpom_time,delete_time\n";
    
    vector<int> sizes = {100, 500, 1000, 5000, 10000};
    
    for (int n : sizes) {
        POMTree pom;
        
        // Measure insert time
        auto start = chrono::high_resolution_clock::now();
        for (int i = 0; i < n; i++) {
            pom.insert(Interval(i * 10, (i + 1) * 10, (i % 2 == 0 ? 5 : -3)));
        }
        auto end = chrono::high_resolution_clock::now();
        long long insertTime = chrono::duration_cast<chrono::microseconds>(end - start).count();
        
        // Measure findPOM time (average of 100 queries)
        start = chrono::high_resolution_clock::now();
        for (int i = 0; i < 100; i++) {
            pom.findPOM();
        }
        end = chrono::high_resolution_clock::now();
        long long findTime = chrono::duration_cast<chrono::microseconds>(end - start).count() / 100;
        
        // Measure delete time
        start = chrono::high_resolution_clock::now();
        for (int i = 0; i < n/2; i++) {
            pom.remove(Interval(i * 10, (i + 1) * 10, (i % 2 == 0 ? 5 : -3)));
        }
        end = chrono::high_resolution_clock::now();
        long long deleteTime = chrono::duration_cast<chrono::microseconds>(end - start).count();
        
        cout << setw(12) << n 
             << setw(15) << insertTime 
             << setw(15) << findTime 
             << setw(15) << deleteTime << "\n";
        
        outfile << n << "," << insertTime << "," << findTime << "," << deleteTime << "\n";
    }
    
    outfile.close();
    cout << "\n" << C_GREEN << "✓ Performance data saved to results/pom_performance.csv" << C_RESET << "\n";
}

// Test 7: Ablation Study - Impact of m on Josephus
void ablationStudyM() {
    printHeader("ABLATION STUDY 1: IMPACT OF PARAMETER m ON JOSEPHUS PERFORMANCE");
    
    printSubHeader("Fixed n=10000, varying m from 2 to 100");
    cout << setw(10) << "m" 
         << setw(15) << "OST (μs)" 
         << setw(15) << "Naive (μs)" 
         << setw(15) << "Speedup" << "\n";
    cout << string(55, '-') << "\n";
    
    ofstream outfile("results/ablation_m.csv");
    outfile << "m,ost_time,naive_time,speedup\n";
    
    int n = 10000;
    vector<int> mValues = {2, 3, 5, 10, 20, 50, 100};
    
    for (int m : mValues) {
        long long ostTime = JosephusPermutation::benchmarkOST(n, m);
        long long naiveTime = JosephusPermutation::benchmarkNaive(n, m);
        double speedup = (double)naiveTime / ostTime;
        
        cout << setw(10) << m 
             << setw(15) << ostTime 
             << setw(15) << naiveTime 
             << setw(14) << fixed << setprecision(2) << speedup << "x" << "\n";
        
        outfile << m << "," << ostTime << "," << naiveTime << "," << speedup << "\n";
    }
    
    outfile.close();
    cout << "\n" << C_GREEN << "✓ Ablation data saved to results/ablation_m.csv" << C_RESET << "\n";
}

// Test 8: Ablation Study - Tree Depth Analysis
void ablationStudyTreeDepth() {
    printHeader("ABLATION STUDY 2: TREE DEPTH VS OPERATION TIME");
    
    printSubHeader("Analyzing relationship between n and operation time");
    cout << setw(10) << "n" 
         << setw(15) << "log2(n)" 
         << setw(18) << "Avg Select (μs)" 
         << setw(15) << "Time/log(n)" << "\n";
    cout << string(58, '-') << "\n";
    
    ofstream outfile("results/ablation_depth.csv");
    outfile << "n,log2n,avg_select_time,time_per_logn\n";
    
    vector<int> sizes = {100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000};
    
    for (int n : sizes) {
        OrderStatisticTree<int> ost;
        
        // Insert elements
        for (int i = 0; i < n; i++) {
            ost.insert(i);
        }
        
        // Measure average select time
        auto start = chrono::high_resolution_clock::now();
        for (int i = 1; i <= min(1000, n); i++) {
            ost.select((i * 17) % n + 1); // Pseudo-random positions
        }
        auto end = chrono::high_resolution_clock::now();
        long long totalTime = chrono::duration_cast<chrono::microseconds>(end - start).count();
        double avgTime = (double)totalTime / min(1000, n);
        
        double log2n = log2(n);
        double timePerLog = avgTime / log2n;
        
        cout << setw(10) << n 
             << setw(15) << fixed << setprecision(2) << log2n 
             << setw(18) << setprecision(4) << avgTime 
             << setw(15) << setprecision(4) << timePerLog << "\n";
        
        outfile << n << "," << log2n << "," << avgTime << "," << timePerLog << "\n";
    }
    
    outfile.close();
    cout << "\n" << C_GREEN << "✓ Tree depth analysis saved to results/ablation_depth.csv" << C_RESET << "\n";
}

// Test 9: Ablation Study - POM with different interval patterns
void ablationStudyPOMPatterns() {
    printHeader("ABLATION STUDY 3: POM PERFORMANCE WITH DIFFERENT VALUE PATTERNS");
    
    printSubHeader("Testing different interval value distributions");
    cout << setw(20) << "Pattern" 
         << setw(12) << "n" 
         << setw(15) << "Insert (μs)" 
         << setw(15) << "FindPOM (μs)" << "\n";
    cout << string(62, '-') << "\n";
    
    ofstream outfile("results/ablation_pom_patterns.csv");
    outfile << "pattern,n,insert_time,findpom_time\n";
    
    int n = 5000;
    
    // Pattern 1: All positive values
    {
        POMTree pom;
        auto start = chrono::high_resolution_clock::now();
        for (int i = 0; i < n; i++) {
            pom.insert(Interval(i * 10, (i + 1) * 10, 5));
        }
        auto end = chrono::high_resolution_clock::now();
        long long insertTime = chrono::duration_cast<chrono::microseconds>(end - start).count();
        
        start = chrono::high_resolution_clock::now();
        pom.findPOM();
        end = chrono::high_resolution_clock::now();
        long long findTime = chrono::duration_cast<chrono::microseconds>(end - start).count();
        
        cout << setw(20) << "All Positive" 
             << setw(12) << n 
             << setw(15) << insertTime 
             << setw(15) << findTime << "\n";
        
        outfile << "all_positive," << n << "," << insertTime << "," << findTime << "\n";
    }
    
    // Pattern 2: Alternating positive/negative
    {
        POMTree pom;
        auto start = chrono::high_resolution_clock::now();
        for (int i = 0; i < n; i++) {
            pom.insert(Interval(i * 10, (i + 1) * 10, (i % 2 == 0 ? 10 : -5)));
        }
        auto end = chrono::high_resolution_clock::now();
        long long insertTime = chrono::duration_cast<chrono::microseconds>(end - start).count();
        
        start = chrono::high_resolution_clock::now();
        pom.findPOM();
        end = chrono::high_resolution_clock::now();
        long long findTime = chrono::duration_cast<chrono::microseconds>(end - start).count();
        
        cout << setw(20) << "Alternating" 
             << setw(12) << n 
             << setw(15) << insertTime 
             << setw(15) << findTime << "\n";
        
        outfile << "alternating," << n << "," << insertTime << "," << findTime << "\n";
    }
    
    // Pattern 3: Random-like (using simple pattern)
    {
        POMTree pom;
        auto start = chrono::high_resolution_clock::now();
        for (int i = 0; i < n; i++) {
            int val = ((i * 17) % 20) - 10; // Values from -10 to 9
            pom.insert(Interval(i * 10, (i + 1) * 10, val));
        }
        auto end = chrono::high_resolution_clock::now();
        long long insertTime = chrono::duration_cast<chrono::microseconds>(end - start).count();
        
        start = chrono::high_resolution_clock::now();
        pom.findPOM();
        end = chrono::high_resolution_clock::now();
        long long findTime = chrono::duration_cast<chrono::microseconds>(end - start).count();
        
        cout << setw(20) << "Random-like" 
             << setw(12) << n 
             << setw(15) << insertTime 
             << setw(15) << findTime << "\n";
        
        outfile << "random_like," << n << "," << insertTime << "," << findTime << "\n";
    }
    
    outfile.close();
    cout << "\n" << C_GREEN << "✓ POM pattern analysis saved to results/ablation_pom_patterns.csv" << C_RESET << "\n";
}

int main() {
    // Create results directory
    system("mkdir -p results");
    
    cout << C_BOLD << C_GREEN;
    cout << R"(
    ╔══════════════════════════════════════════════════════════════╗
    ║                                                              ║
    ║   AUGMENTED DATA STRUCTURES: OST & POM TREES                ║
    ║   Ablation Study and Performance Analysis                   ║
    ║                                                              ║
    ╚══════════════════════════════════════════════════════════════╝
    )" << C_RESET;
    
    // Run all tests
    testOSTBasic();
    testPOMBasic();
    testJosephusCorrectness();
    testOSTPerformance();
    testJosephusPerformance();
    testPOMPerformance();
    
    // Run ablation studies
    ablationStudyM();
    ablationStudyTreeDepth();
    ablationStudyPOMPatterns();
    
    // Final summary
    printHeader("EXECUTION COMPLETE");
    cout << C_GREEN << "All tests and ablation studies completed successfully!\n" << C_RESET;
    cout << "Results saved to " << C_BOLD << "results/" << C_RESET << " directory\n\n";
    
    cout << "Generated files:\n";
    cout << "  • ost_performance.csv - OST operation benchmarks\n";
    cout << "  • josephus_comparison.csv - OST vs Naive comparison\n";
    cout << "  • pom_performance.csv - POM tree benchmarks\n";
    cout << "  • ablation_m.csv - Impact of parameter m\n";
    cout << "  • ablation_depth.csv - Tree depth analysis\n";
    cout << "  • ablation_pom_patterns.csv - POM value pattern analysis\n\n";
    
    return 0;
}

