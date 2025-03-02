#include "SM2Scheduler.hpp"
#include <algorithm>
#include <ctime>

/** TODO: BETTER DEFAULTS
 * Basic SM-2 logic:
 *
 * If quality < 3 => reset interval to 1 day (or learning step).
 * Else:
 *   if interval == 0 => set to 1
 *   else if interval == 1 => set to 6
 *   else interval = round(interval * EF)
 *
 * EF = EF + (0.1 - (5 - quality) * (0.08 + (5 - quality)*0.02))
 * if EF < 1.3 => EF = 1.3
 *
 * next due = now + interval * 1 day
 *
 * Lapses, leeches, etc. can be extended if needed.
 */

SM2Scheduler::SM2Scheduler() {}
SM2Scheduler::~SM2Scheduler() {}

void SM2Scheduler::updateCard(Card &card, int quality) {
  quality = std::max(0, std::min(quality, 5));

  double ef = card.easeFactor();
  int ivl = card.interval();

  if (quality < 3) {
    // reset
    ivl = 1;
  } else {
    switch (ivl) {
    case 0:
      ivl = 1;
      break;
    case 1:
      ivl = 6;
      break;
    default:
      ivl = (int)(ivl * ef + 0.5);
      break;
    }
    ef = ef + (0.1 - (5 - quality) * (0.08 + (5 - quality) * 0.02));
    if (ef < 1.3)
      ef = 1.3;
  }

  card.setInterval(ivl);
  card.setEaseFactor(ef);

  time_t now = std::time(nullptr);
  time_t next = now + (ivl * 24 * 60 * 60);
  card.setDueDate(next);

  // potential leech detection
  // e.g. if user fails multiple times => card.setSuspended(true);
  if (quality == 0) {
    // if fail count high => card.setSuspended(true);
    // TODO
  }
}
