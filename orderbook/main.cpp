#include "order_book.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

class InteractiveOrderBook {
private:
    OrderBook book;
    uint64_t next_order_id = 1;

public:
    void run() {
        initialize_default_orders();

        std::cout << "=== INTERACTIVE ORDER BOOK ===\n";
        std::cout << "Default orders loaded. Current book state:\n";
        book.print_book();

        while (true) {
            print_menu();
            std::string input;
            std::getline(std::cin, input);

            if (input.empty()) continue;

            char choice = std::tolower(input[0]);

            switch (choice) {
                case 'a':
                    add_order_interactive();
                    break;
                case 'c':
                    cancel_order_interactive();
                    break;
                case 'm':
                    amend_order_interactive();
                    break;
                case 'v':
                    book.print_book();
                    break;
                case 't':
                    test_matching_scenario();
                    break;
                default:
                    std::cout << "Invalid choice. Please try again.\n";
            }
        }
    }

private:
   void initialize_default_orders() {
    std::vector<Order> default_orders = {

        {next_order_id++, true, OrderType::LIMIT, 100.0, 1000, 0},   // Large order at key level
        {next_order_id++, true, OrderType::LIMIT, 99.9, 250, 0},     // Small order just below
        {next_order_id++, true, OrderType::LIMIT, 99.8, 500, 0},     // Medium order
        {next_order_id++, true, OrderType::LIMIT, 99.7, 750, 0},     // Large order
        {next_order_id++, true, OrderType::LIMIT, 99.6, 300, 0},     // Medium order
        {next_order_id++, true, OrderType::LIMIT, 99.5, 1200, 0},    // Very large order - support level
        {next_order_id++, true, OrderType::LIMIT, 99.4, 400, 0},     // Medium order
        {next_order_id++, true, OrderType::LIMIT, 99.3, 600, 0},     // Large order
        {next_order_id++, true, OrderType::LIMIT, 99.2, 200, 0},     // Small order
        {next_order_id++, true, OrderType::LIMIT, 99.1, 450, 0},     // Medium order
        {next_order_id++, true, OrderType::LIMIT, 99.0, 800, 0},     // Large order - major support

        {next_order_id++, false, OrderType::LIMIT, 100.1, 200, 0},   // Small order just above
        {next_order_id++, false, OrderType::LIMIT, 100.2, 450, 0},   // Medium order
        {next_order_id++, false, OrderType::LIMIT, 100.3, 600, 0},   // Large order
        {next_order_id++, false, OrderType::LIMIT, 100.4, 300, 0},   // Medium order
        {next_order_id++, false, OrderType::LIMIT, 100.5, 800, 0},   // Large order
        {next_order_id++, false, OrderType::LIMIT, 100.6, 400, 0},   // Medium order
        {next_order_id++, false, OrderType::LIMIT, 100.7, 950, 0},   // Very large order - resistance level
        {next_order_id++, false, OrderType::LIMIT, 100.8, 250, 0},   // Small order
        {next_order_id++, false, OrderType::LIMIT, 100.9, 550, 0},   // Large order
        {next_order_id++, false, OrderType::LIMIT, 101.0, 1200, 0},  // Very large order - major resistance


        // Cluster orders at specific levels
        {next_order_id++, true, OrderType::LIMIT, 99.5, 300, 0},     // Additional order at support
        {next_order_id++, true, OrderType::LIMIT, 99.5, 200, 0},     // Another at same level
        {next_order_id++, false, OrderType::LIMIT, 101.0, 400, 0},   // Additional order at resistance
        {next_order_id++, false, OrderType::LIMIT, 101.0, 350, 0}    // Another at same level
    };

    std::cout << "Loading " << default_orders.size() << " default orders...\n";

    for (const auto& order : default_orders) {
        auto trades = book.add_order(order);
        if (!trades.empty()) {
            print_trades(trades);
        }
    }

    std::cout << "Market initialized with " << default_orders.size() << " orders\n";
}

    void print_menu() {
        std::cout << "\n=== MENU ===\n";
        std::cout << "A - Add new order\n";
        std::cout << "C - Cancel order\n";
        std::cout << "M - Amend order\n";
        std::cout << "V - View order book\n";
        std::cout << "T - Test matching scenario\n";
        std::cout << "Choice: ";
    }

    void add_order_interactive() {
        std::cout << "\n=== ADD NEW ORDER ===\n";

        bool is_buy;
        OrderType order_type;
        double price = 0.0;
        uint64_t quantity;

        std::cout << "Order type (M for Market, L for Limit): ";
        std::string type_input;
        std::getline(std::cin, type_input);

        if (std::tolower(type_input[0]) == 'm') {
            order_type = OrderType::MARKET;
            std::cout << "Creating MARKET order\n";
        } else if (std::tolower(type_input[0]) == 'l') {
            order_type = OrderType::LIMIT;
            std::cout << "Creating LIMIT order\n";
        } else {
            std::cout << "Invalid order type.\n";
            return;
        }

        std::cout << "Order side (B for Buy, S for Sell): ";
        std::string side_input;
        std::getline(std::cin, side_input);

        if (std::tolower(side_input[0]) == 'b') {
            is_buy = true;
            std::cout << "Creating BUY order\n";
        } else if (std::tolower(side_input[0]) == 's') {
            is_buy = false;
            std::cout << "Creating SELL order\n";
        } else {
            std::cout << "Invalid order side.\n";
            return;
        }

        if (order_type == OrderType::LIMIT) {
            std::cout << "Price: ";
            std::string price_input;
            std::getline(std::cin, price_input);
            try {
                price = std::stod(price_input);
            } catch (...) {
                std::cout << "Invalid price.\n";
                return;
            }
        } else {
            double best_bid = book.get_best_bid();
            double best_ask = book.get_best_ask();
            if (is_buy) {
                std::cout << "Market BUY will execute at best ask: " << best_ask << "\n";
            } else {
                std::cout << "Market SELL will execute at best bid: " << best_bid << "\n";
            }
        }

        std::cout << "Quantity: ";
        std::string qty_input;
        std::getline(std::cin, qty_input);
        try {
            quantity = std::stoul(qty_input);
        } catch (...) {
            std::cout << "Invalid quantity.\n";
            return;
        }

        Order new_order{next_order_id++, is_buy, order_type, price, quantity, 0};
        std::cout << "Adding order ID: " << new_order.order_id << " (" 
                  << (order_type == OrderType::MARKET ? "MARKET" : "LIMIT") << ")\n";

        auto trades = book.add_order(new_order);

        if (!trades.empty()) {
            std::cout << "TRADES EXECUTED:\n";
            print_trades(trades);
        } else {
            if (order_type == OrderType::MARKET) {
                std::cout << "Market order fully executed or no liquidity available.\n";
            } else {
                std::cout << "Limit order placed in the book.\n";
            }
        }

        book.print_book();
    }

    void cancel_order_interactive() {
        std::cout << "\n=== CANCEL ORDER ===\n";
        std::cout << "Order ID to cancel: ";

        std::string input;
        std::getline(std::cin, input);

        try {
            uint64_t order_id = std::stoul(input);
            if (book.cancel_order(order_id)) {
                std::cout << "Order " << order_id << " cancelled successfully.\n";
            } else {
                std::cout << "Order " << order_id << " not found.\n";
            }
        } catch (...) {
            std::cout << "Invalid order ID.\n";
        }

        book.print_book();
    }

    void amend_order_interactive() {
        std::cout << "\n=== AMEND ORDER ===\n";
        std::cout << "Order ID to amend: ";

        std::string id_input;
        std::getline(std::cin, id_input);

        try {
            uint64_t order_id = std::stoul(id_input);

            if (!book.order_exists(order_id)) {
                std::cout << "Order " << order_id << " not found.\n";
                return;
            }

            std::cout << "New price: ";
            std::string price_input;
            std::getline(std::cin, price_input);
            double new_price = std::stod(price_input);

            std::cout << "New quantity: ";
            std::string qty_input;
            std::getline(std::cin, qty_input);
            uint64_t new_quantity = std::stoul(qty_input);

            if (book.amend_order(order_id, new_price, new_quantity)) {
                std::cout << "Order " << order_id << " amended successfully.\n";
            } else {
                std::cout << "Failed to amend order " << order_id << ".\n";
            }
        } catch (...) {
            std::cout << "Invalid input.\n";
        }

        book.print_book();
    }


    void test_matching_scenario() {
    std::cout << "\n=== TEST MATCHING SCENARIOS ===\n";
    std::cout << "1. Market Buy Order\n";
    std::cout << "2. Market Sell Order\n";
    std::cout << "3. Aggressive Limit Buy (Cross Spread)\n";
    std::cout << "4. Aggressive Limit Sell (Cross Spread)\n";
    std::cout << "Choice: ";

    std::string input;
    std::getline(std::cin, input);

    double best_bid = book.get_best_bid();
    double best_ask = book.get_best_ask();

    if (best_bid == 0 || best_ask == 0) {
        std::cout << "No bids or asks in the book.\n";
        return;
    }

    double mid_price = (best_bid + best_ask) / 2.0;

    switch (input[0]) {
        case '1': {
            Order market_buy{next_order_id++, true, OrderType::MARKET, 0, 500, 0};
            std::cout << "Adding MARKET BUY for 500 units\n";
            auto trades = book.add_order(market_buy);
            print_trades(trades);
            break;
        }
        case '2': {
            Order market_sell{next_order_id++, false, OrderType::MARKET, 0, 500, 0};
            std::cout << "Adding MARKET SELL for 500 units\n";
            auto trades = book.add_order(market_sell);
            print_trades(trades);
            break;
        }
        case '3': {
            Order aggressive_buy{next_order_id++, true, OrderType::LIMIT, best_ask + 0.3, 800, 0};
            std::cout << "Adding aggressive LIMIT BUY at " << aggressive_buy.price 
                      << " (above best ask of " << best_ask << ")\n";
            auto trades = book.add_order(aggressive_buy);
            print_trades(trades);
            break;
        }
        case '4': {
            Order aggressive_sell{next_order_id++, false, OrderType::LIMIT, best_bid - 0.3, 800, 0};
            std::cout << "Adding aggressive LIMIT SELL at " << aggressive_sell.price 
                      << " (below best bid of " << best_bid << ")\n";
            auto trades = book.add_order(aggressive_sell);
            print_trades(trades);
            break;
        }
        default:
            std::cout << "Invalid choice.\n";
            return;
    }

    book.print_book();
}


    void print_trades(const std::vector<Trade>& trades) {
        if (trades.empty()) return;

        std::cout << "*************** TRADES EXECUTED ***************\n";
        for (const auto& trade : trades) {
            std::cout << "* BUY: " << std::setw(6) << trade.buy_order_id 
                      << " SELL: " << std::setw(6) << trade.sell_order_id
                      << " PRICE: " << std::setw(8) << std::fixed << std::setprecision(2) << trade.price
                      << " QTY: " << std::setw(6) << trade.quantity << " *\n";
        }
        std::cout << "***********************************************\n";
    }
};

int main() {
    InteractiveOrderBook interactive_book;
    interactive_book.run();
    return 0;
}