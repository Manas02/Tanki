#ifndef TANKI_DECK_HPP
#define TANKI_DECK_HPP

#include "Card.hpp"
#include <string>
#include <vector>

class Deck {
public:
  Deck(const std::string &name);
  ~Deck();

  std::string name() const;
  void setName(const std::string &n);

  void addCard(const Card &c);
  void updateCard(const Card &c);

  // For "delete" we might want direct setCards
  void setCards(const std::vector<Card> &cards);

  std::vector<Card> cards() const;
  std::vector<Card> getDueCards();

private:
  std::string _name;
  std::vector<Card> _cards;
};

#endif // TANKI_DECK_HPP
