# Quantitative Market-Making Simulation

An interactive, console-based market-making engine built in C++ to simulate high-frequency trading dynamics. This project models the core responsibilities of a quantitative liquidity provider, focusing on execution latency, inventory risk management, and dynamic bid-ask spread pricing.

## 🚀 Overview & Objectives
**Objective:** Maintain a profitable market-making strategy by continuously quoting two-way prices while strictly managing inventory limits and minimizing execution latency.
**Tools:** C++, Standard Template Library (`<chrono>`, `<vector>`, `<random>`).
**Impact:** Demonstrates low-latency systems engineering concepts, real-time risk management, and the practical application of C++ in quantitative financial simulations.

## ⚙️ Core Engine Features

*   **Execution Latency Tracking:** Utilizes `std::chrono` to measure the exact time taken to submit quotes. Exceeding the built-in latency threshold triggers a `[TIMEOUT]` penalty, simulating the market moving before the order reaches the exchange.
*   **Dynamic Inventory Management:** Tracks real-time open positions (long/short). Quotes must be dynamically skewed to attract offsetting trades and avoid breaching strict inventory limits.
*   **Two-Way Quoting (Bid/Ask):** Requires simultaneous calculation and submission of bid and ask prices to capture the spread while accounting for market volatility.
*   **PnL Calculation:** Continuously calculates total Profit and Loss (P&L) across rounds, factoring in captured spreads and execution latency penalties.

## 💻 Compilation & Execution

Navigate to the project directory and compile the source code using a standard C++ compiler (e.g., GCC/MinGW):

g++ MarketGame.cpp -o MarketGame
./MarketGame
