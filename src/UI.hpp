#ifndef TANKI_UI_HPP
#define TANKI_UI_HPP

#include "Card.hpp"
#include "Deck.hpp"
#include <memory>
#include <ncurses.h>
#include <string>
#include <vector>

/**
 * UI handling:
 * - Fancy start screen
 * - Single-line prompt
 * - Browse
 * - "promptIndexToDelete" for picking a card to delete
 */
class UI {
public:
  UI();
  ~UI();

  void init();
  void shutdown();
  void setColorTheme(const std::string &theme);

  // Fancy start screens
  int startScreenWithDecks(const std::vector<std::shared_ptr<Deck>> &decks);
  int startScreenNoDecks();

  // Main menu
  void drawMainMenu(const std::string &deckName);

  // Single-line input
  std::string promptString(const std::string &message);

  // "browse"
  void browseDeck(std::shared_ptr<Deck> deck);

  // Let user pick a card index to delete
  int promptIndexToDelete(const std::vector<Card> &cards,
                          const std::string &deckName);

  // Deck selection
  int showDeckSelection(const std::vector<std::shared_ptr<Deck>> &decks);

  // Messages & large text
  void showMessage(const std::string &message);
  void showLongText(const std::string &title, const std::string &content);

  // Review UI
  bool reviewCard(Card &card, bool isCram);

private:
  WINDOW *mainWin;
  WINDOW *statusWin;

  // color pairs
  int colorMenu;
  int colorBorder;
  int colorFront;
  int colorBack;
  int colorNormal;
  int colorTitle;

  void drawStatusLine(const std::string &text);
  void clearAll();
  void smallTransition();

  // optional box w/ title
  void drawBoxTitle(WINDOW *win, const std::string &title);
};

#endif // TANKI_UI_HPP
