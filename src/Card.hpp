#ifndef TANKI_CARD_HPP
#define TANKI_CARD_HPP

#include <ctime>
#include <set>
#include <string>

class Card {
public:
  Card();
  Card(const std::string &front, const std::string &back);
  ~Card();

  int id() const;
  const std::string &front() const;
  const std::string &back() const;

  time_t dueDate() const;
  void setDueDate(time_t t);

  bool isSuspended() const;
  void setSuspended(bool s);

  int interval() const;
  void setInterval(int i);

  double easeFactor() const;
  void setEaseFactor(double e);

  int lastRating() const;
  void setLastRating(int r);

  void setFront(const std::string &f);
  void setBack(const std::string &b);

  void setTags(const std::string &tags);
  bool hasTag(const std::string &tag) const;
  std::string tagsString() const;

private:
  int _id;
  std::string _front;
  std::string _back;
  time_t _dueDate;
  bool _suspended;
  int _interval;
  double _easeFactor;
  int _lastRating;

  std::set<std::string> _tags;
};

#endif // TANKI_CARD_HPP
