#include <iostream>
#include <random>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <vector>
#include <numeric>

// Structure to store data for every single round
struct TradeRecord {
    int round;
    double latency;
    double pnl;
};

class MarketGame {
private:
    int true_value;
    int user_position;
    double user_pnl;
    int round_count;
    const int MAX_INVENTORY = 3;
    
    std::vector<TradeRecord> trade_history; // In-memory database of your session

public:
    MarketGame() : user_position(0), user_pnl(0.0), true_value(0), round_count(1) {}

    void roll_dice() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(1, 6);
        true_value = distrib(gen) + distrib(gen);
    }

    void check_risk_limits() {
        if (std::abs(user_position) >= MAX_INVENTORY) {
            std::cout << "\n[ALERT] RISK LIMIT BREACHED: Inventory at " << user_position << " contracts!\n";
            double liquidation_loss = 0;
            if (user_position > 0) {
                liquidation_loss = (2.0 - 7.0) * user_position;
            } else {
                liquidation_loss = (7.0 - 12.0) * std::abs(user_position);
            }
            user_pnl += liquidation_loss;
            user_position = 0;
            std::cout << "-> Desk flattened position. Liquidation P&L impact: $" 
                      << std::fixed << std::setprecision(2) << liquidation_loss << "\n";
            
            // Record the liquidation penalty as a separate event in history
            trade_history.push_back({round_count, 0.0, liquidation_loss});
        }
    }

    void play_round() {
        roll_dice();
        double current_round_pnl = 0.0; // Track P&L for just this round

        std::cout << "\n================ ROUND " << round_count << " ================\n";
        std::cout << "Inventory : " << user_position << " (Limit: +/-" << MAX_INVENTORY << ") | Total P&L: $" 
                  << std::fixed << std::setprecision(2) << user_pnl << "\n";
        std::cout << "Quote Two-Way (Bid Ask, e.g., 6.5 7.5): ";

        auto start_time = std::chrono::high_resolution_clock::now();
        double bid, ask;
        if (!(std::cin >> bid >> ask)) {
            std::cout << "\nInvalid input stream. Terminating session.\n";
            exit(0);
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time;
        double latency_sec = elapsed.count();

        if (latency_sec > 5.0) {
            std::cout << "\n[TIMEOUT] Latency: " << std::fixed << std::setprecision(2) << latency_sec << "s. Market moved.\n";
            current_round_pnl = -1.50;
            user_pnl += current_round_pnl;
            trade_history.push_back({round_count, latency_sec, current_round_pnl});
            round_count++;
            return;
        }

        if (bid >= ask) {
            std::cout << "[REJECTED] Inverted market.\n";
            trade_history.push_back({round_count, latency_sec, 0.0});
            round_count++;
            return;
        }

        double spread = ask - bid;
        if (spread > 3.0) {
            std::cout << "[PENALTY] Spread too wide. -$1.00 inactivity fee.\n";
            current_round_pnl = -1.00;
            user_pnl += current_round_pnl;
            trade_history.push_back({round_count, latency_sec, current_round_pnl});
            round_count++;
            return;
        }

        std::cout << "\n[ORDER FLOW] Matching quotes...\n";
        if (true_value > ask) {
            int fill_size = (true_value - ask >= 3.0) ? 2 : 1;
            std::cout << "-> Aggressor BOUGHT " << fill_size << " at your Ask: $" << ask << "\n";
            user_position -= fill_size;
            current_round_pnl = (ask - true_value) * fill_size;
        } 
        else if (true_value < bid) {
            int fill_size = (bid - true_value >= 3.0) ? 2 : 1;
            std::cout << "-> Aggressor SOLD " << fill_size << " at your Bid: $" << bid << "\n";
            user_position += fill_size;
            current_round_pnl = (true_value - bid) * fill_size;
        } 
        else {
            std::cout << "-> Passive edge captured on spread.\n";
            current_round_pnl = spread / 2.0;
        }

        std::cout << "-> Settlement: " << true_value << " | Round P&L: $" << current_round_pnl << "\n";
        user_pnl += current_round_pnl;
        
        trade_history.push_back({round_count, latency_sec, current_round_pnl});
        
        check_risk_limits();
        round_count++;
    }

    void print_analytics() {
        std::cout << "\n================ POST-TRADE RISK ANALYTICS ================\n";
        if (trade_history.empty()) {
            std::cout << "No trade data recorded.\n";
            return;
        }

        int winning_trades = 0;
        double total_latency = 0.0;
        double pnl_variance = 0.0;
        double mean_pnl = user_pnl / trade_history.size();

        for (const auto& trade : trade_history) {
            if (trade.pnl > 0) winning_trades++;
            total_latency += trade.latency;
            pnl_variance += std::pow(trade.pnl - mean_pnl, 2);
        }

        double win_rate = (static_cast<double>(winning_trades) / trade_history.size()) * 100.0;
        double avg_latency = total_latency / trade_history.size();
        double std_dev = std::sqrt(pnl_variance / trade_history.size());

        std::cout << "Total Rounds Played   : " << trade_history.size() << "\n";
        std::cout << "Final Net Inventory   : " << user_position << " contracts\n";
        std::cout << "Final Realized P&L    : $" << std::fixed << std::setprecision(2) << user_pnl << "\n";
        std::cout << "-----------------------------------------------------------\n";
        std::cout << "Win Rate              : " << win_rate << "%\n";
        std::cout << "Avg Execution Latency : " << avg_latency << " seconds\n";
        std::cout << "P&L Std Deviation     : $" << std_dev << " (Volatility Metric)\n";
        
        if (std_dev > 0) {
            std::cout << "Sharpe-Proxy Ratio    : " << (mean_pnl / std_dev) << "\n";
        }
        std::cout << "===========================================================\n";
    }
};

int main() {
    std::cout << "=================================================\n";
    std::cout << "   Automated C++ Market-Making Simulation Engine  \n";
    std::cout << "=================================================\n";

    MarketGame session;

    for (int i = 0; i < 5; ++i) {
        session.play_round();
    }

    session.print_analytics();
    return 0;
}