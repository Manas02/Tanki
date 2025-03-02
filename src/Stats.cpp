#include "Stats.hpp"
#include <ctime>
#include <sstream>

std::string Stats::generateStats(std::shared_ptr<Deck> deck) {
  if (!deck)
    return "No deck selected.";
  auto cards = deck->cards();

  int total = (int)cards.size();
  int suspended = 0;
  int newCount = 0;
  int learning = 0;
  int mature = 0;

  for (auto &c : cards) {
    if (c.isSuspended())
      suspended++;
    if (c.interval() == 0) {
      newCount++;
    } else if (c.interval() < 21) {
      learning++;
    } else {
      mature++;
    }
  }

  std::ostringstream oss;
  oss << "Deck: " << deck->name() << "\n";
  oss << "Total: " << total << "\n";
  oss << "New: " << newCount << "\n";
  oss << "Learning (<21d): " << learning << "\n";
  oss << "Mature: " << mature << "\n";
  oss << "Suspended: " << suspended << "\n\n";
  return oss.str();
}

std::string Stats::generateScheduleInfo(std::shared_ptr<Deck> deck) {
  if (!deck)
    return "No deck.";
  auto cards = deck->cards();
  int next7[7] = {0};
  time_t now = std::time(nullptr);
  const int DAYSEC = 24 * 60 * 60;

  for (auto &c : cards) {
    if (!c.isSuspended()) {
      double diff = difftime(c.dueDate(), now);
      if (diff <= 0) {
        next7[0]++;
      } else {
        int d = (int)(diff / DAYSEC);
        if (d < 7) {
          next7[d]++;
        }
      }
    }
  }
  std::ostringstream oss;
  oss << "Cards due in next 7 days:\n";
  for (int i = 0; i < 7; i++) {
    oss << "Day " << i << ": " << next7[i] << "\n";
  }
  return oss.str();
}
