#pragma once

/**
 * @file Enums.hpp
 * @brief Defines the enumeration classes for the xAPI.
 *
 * This file contains the definition of the enumeration classes used in the XAPI.
 */

namespace xapi
{

/**
 * @enum TradeCmd
 * @brief Represents trading commands in the xAPI.
 */
enum class TradeCmd
{
    BUY = 0,        // buy
    SELL = 1,       // sell
    BUY_LIMIT = 2,  // buy limit
    SELL_LIMIT = 3, // sell limit
    BUY_STOP = 4,   // buy stop
    SELL_STOP = 5,  // sell stop
    BALANCE = 6,    // Read only. Used in getTradesHistory for manager's deposit/withdrawal operations (profit>0 for
                    // deposit, profit<0 for withdrawal).
    CREDIT = 7      // Read only
};

/**
 * @enum TradeType
 * @brief Represents the type of a trade in the xAPI.
 */
enum class TradeType
{
    OPEN = 0,    // order open, used for opening orders
    PENDING = 1, // order pending, only used in the streaming getTrades command
    CLOSE = 2,   // order close
    MODIFY = 3,  // order modify, only used in the tradeTransaction command
    DELETE = 4   // order delete, only used in the tradeTransaction command
};

/**
 * @enum TradeStatus
 * @brief Represents the status of a trade in the xAPI.
 */
enum class TradeStatus
{
    ERROR = 0,    // error
    PENDING = 1,  // pending
    ACCEPTED = 3, // The transaction has been executed successfully
    REJECTED = 4  // The transaction has been rejected
};

/**
 * @enum PeriodCode
 * @brief Represents the period code for a time period in the xAPI.
 */
enum class PeriodCode
{
    PERIOD_M1 = 1,     // 1 minute
    PERIOD_M5 = 5,     // 5 minutes
    PERIOD_M15 = 15,   // 15 minutes
    PERIOD_M30 = 30,   // 30 minutes
    PERIOD_H1 = 60,    // 60 minutes (1 hour)
    PERIOD_H4 = 240,   // 240 minutes (4 hours)
    PERIOD_D1 = 1440,  // 1440 minutes (1 day)
    PERIOD_W1 = 10080, // 10080 minutes (1 week)
    PERIOD_MN1 = 43200 // 43200 minutes (30 days)
};

} // namespace xapi
