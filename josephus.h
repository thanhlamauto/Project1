#ifndef JOSEPHUS_H
#define JOSEPHUS_H

#include "ost.h"
#include <vector>
#include <chrono>

/**
 * Josephus Permutation Generator
 * Uses Order Statistic Tree to efficiently generate Josephus permutation
 * 
 * Problem: n people stand in a circle. Starting from position 0,
 * every m-th person is eliminated. Generate the elimination order.
 * 
 * Naive approach: O(n*m) using array simulation
 * OST approach: O(n log n) using select and delete operations
 */

class JosephusPermutation {
public:
    // Generate Josephus permutation using OST (efficient)
    static std::vector<int> generateOST(int n, int m) {
        OrderStatisticTree<int> ost;
        
        // Initialize tree with positions 0 to n-1
        for (int i = 0; i < n; i++) {
            ost.insert(i);
        }
        
        std::vector<int> result;
        int current = 0;
        
        while (!ost.empty()) {
            // Calculate next position to eliminate
            current = (current + m - 1) % ost.size();
            
            // Select and remove the element at position (current + 1)
            // +1 because select is 1-indexed
            int eliminated = ost.select(current + 1);
            result.push_back(eliminated);
            ost.remove(eliminated);
            
            // Update current position for next iteration
            if (!ost.empty()) {
                current = current % ost.size();
            }
        }
        
        return result;
    }
    
    // Generate Josephus permutation using naive approach (for comparison)
    static std::vector<int> generateNaive(int n, int m) {
        std::vector<bool> alive(n, true);
        std::vector<int> result;
        
        int current = 0;
        int remaining = n;
        
        while (remaining > 0) {
            int count = 0;
            
            // Skip m-1 alive people
            while (count < m) {
                if (alive[current]) {
                    count++;
                    if (count == m) break;
                }
                current = (current + 1) % n;
            }
            
            // Eliminate current person
            alive[current] = false;
            result.push_back(current);
            remaining--;
            
            // Move to next alive person
            if (remaining > 0) {
                current = (current + 1) % n;
                while (!alive[current]) {
                    current = (current + 1) % n;
                }
            }
        }
        
        return result;
    }
    
    // Benchmark OST approach
    static long long benchmarkOST(int n, int m) {
        auto start = std::chrono::high_resolution_clock::now();
        generateOST(n, m);
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    }
    
    // Benchmark naive approach
    static long long benchmarkNaive(int n, int m) {
        auto start = std::chrono::high_resolution_clock::now();
        generateNaive(n, m);
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    }
    
    // Verify that both approaches produce the same result
    static bool verify(int n, int m) {
        std::vector<int> resultOST = generateOST(n, m);
        std::vector<int> resultNaive = generateNaive(n, m);
        return resultOST == resultNaive;
    }
};

#endif // JOSEPHUS_H

