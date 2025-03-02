#include "Card.hpp"
#include <cstdlib>
#include <ctime>

static int globalId = 0;

static int genId() { return ++globalId; }

Card::Card()
    : _id(genId()), _front(""), _back(""), _dueDate(std::time(nullptr)),
      _suspended(false), _interval(0), _easeFactor(2.5), _lastRating(0) {}

Card::Card(const std::string &front, const std::string &back)
    : _id(genId()), _front(front), _back(back), _dueDate(std::time(nullptr)),
      _suspended(false), _interval(0), _easeFactor(2.5), _lastRating(0) {}

Card::~Card() {}

int Card::id() const { return _id; }

const std::string &Card::front() const { return _front; }

const std::string &Card::back() const { return _back; }

time_t Card::dueDate() const { return _dueDate; }
void Card::setDueDate(time_t t) { _dueDate = t; }

bool Card::isSuspended() const { return _suspended; }
void Card::setSuspended(bool s) { _suspended = s; }

int Card::interval() const { return _interval; }
void Card::setInterval(int i) { _interval = i; }

double Card::easeFactor() const { return _easeFactor; }
void Card::setEaseFactor(double e) { _easeFactor = e; }

int Card::lastRating() const { return _lastRating; }
void Card::setLastRating(int r) { _lastRating = r; }

void Card::setFront(const std::string &f) { _front = f; }
void Card::setBack(const std::string &b) { _back = b; }

void Card::setTags(const std::string &tagString) {
  _tags.clear();
  size_t start = 0;
  while (true) {
    size_t pos = tagString.find(',', start);
    if (pos == std::string::npos) {
      std::string t = tagString.substr(start);
      if (!t.empty()) {
        // trim
        t.erase(0, t.find_first_not_of(" \t"));
        t.erase(t.find_last_not_of(" \t") + 1);
        if (!t.empty())
          _tags.insert(t);
      }
      break;
    } else {
      std::string t = tagString.substr(start, pos - start);
      t.erase(0, t.find_first_not_of(" \t"));
      t.erase(t.find_last_not_of(" \t") + 1);
      if (!t.empty())
        _tags.insert(t);
      start = pos + 1;
    }
  }
}

bool Card::hasTag(const std::string &tag) const {
  return _tags.find(tag) != _tags.end();
}
std::string Card::tagsString() const {
  std::string out;
  for (auto &t : _tags) {
    if (!out.empty())
      out += ", ";
    out += t;
  }
  return out;
}
