#ifndef TANKI_STATS_HPP
#define TANKI_STATS_HPP

#include "Deck.hpp"
#include <memory>
#include <string>

class Stats {
public:
  static std::string generateStats(std::shared_ptr<Deck> deck);
  static std::string generateScheduleInfo(std::shared_ptr<Deck> deck);

private:
  // helper(s)
};

#endif // TANKI_STATS_HPP
