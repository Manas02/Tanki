#ifndef TANKI_SM2SCHEDULER_HPP
#define TANKI_SM2SCHEDULER_HPP

#include "Card.hpp"

class SM2Scheduler {
public:
  SM2Scheduler();
  ~SM2Scheduler();

  void updateCard(Card &card, int quality);
};

#endif // TANKI_SM2SCHEDULER_HPP
