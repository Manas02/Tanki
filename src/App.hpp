#ifndef TANKI_APP_HPP
#define TANKI_APP_HPP

#include "Deck.hpp"
#include "UI.hpp"
#include <memory>
#include <string>
#include <vector>

/**
 * Main application controller.
 * - Loads decks
 * - Initial "welcome" screen with logo
 * - Menu for review, cram, browse, import CSV, delete card, switch deck, etc.
 */
class App {
public:
  App();
  ~App();

  void run();

private:
  bool running;
  UI ui;

  // All decks
  std::vector<std::shared_ptr<Deck>> allDecks;
  // Current deck
  std::shared_ptr<Deck> currentDeck;

  // Deck I/O
  void loadDecks();
  void saveDecks();

  // The fancy start screen
  void startScreen();

  // Deck mgmt
  void createDeck();
  void switchDeck();
  void mergeDecks(); // stub
  void splitDeck();  // stub

  // CSV
  void importCSV();
  void exportCSV(); // stub

  // Main actions
  void review();
  void cram();
  void deleteCard(); // NEW: user can delete a card
  void showStats();
  void showSchedule();
  void helpScreen();

  // Stub
  void examDrillMode();
};

#endif // TANKI_APP_HPP
