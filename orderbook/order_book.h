#pragma once
#include <cstdint>
#include <vector>
#include <string>

enum class OrderType {
    LIMIT,
    MARKET
};

struct Order {
    uint64_t order_id;     
    bool is_buy;           
    OrderType order_type;  
    double price;          
    uint64_t quantity;      
    uint64_t timestamp_ns; 
};

struct PriceLevel {
    double price;
    uint64_t total_quantity;
    uint64_t order_count;
};

struct Trade {
    uint64_t buy_order_id;
    uint64_t sell_order_id;
    double price;
    uint64_t quantity;
    uint64_t timestamp_ns;
};

class OrderBook {
private:
    class OrderBookImpl;
    OrderBookImpl* impl;

public:
    OrderBook();
    ~OrderBook();

    std::vector<Trade> add_order(const Order& order);

    bool cancel_order(uint64_t order_id);

    bool amend_order(uint64_t order_id, double new_price, uint64_t new_quantity);

    void get_snapshot(size_t depth, std::vector<PriceLevel>& bids, std::vector<PriceLevel>& asks) const;

    void print_book(size_t depth = 10) const;

    double get_best_bid() const;
    double get_best_ask() const;

    bool order_exists(uint64_t order_id) const;

    void get_price_levels(std::vector<PriceLevel>& bids, std::vector<PriceLevel>& asks) const;
};