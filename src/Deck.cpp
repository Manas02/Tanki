#include "Deck.hpp"
#include <ctime>

Deck::Deck(const std::string &name) : _name(name) {}

Deck::~Deck() {}

std::string Deck::name() const { return _name; }

void Deck::setName(const std::string &n) { _name = n; }

void Deck::addCard(const Card &c) { _cards.push_back(c); }

void Deck::updateCard(const Card &c) {
  // find by ID
  for (auto &ex : _cards) {
    if (ex.id() == c.id()) {
      ex = c;
      return;
    }
  }
}

void Deck::setCards(const std::vector<Card> &cards) { _cards = cards; }

std::vector<Card> Deck::cards() const { return _cards; }

std::vector<Card> Deck::getDueCards() {
  std::vector<Card> due;
  auto now = std::time(nullptr);
  for (auto &c : _cards) {
    if (!c.isSuspended() && c.dueDate() <= now) {
      due.push_back(c);
    }
  }
  return due;
}
