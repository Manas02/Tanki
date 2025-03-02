#include "App.hpp"
#include "FileManager.hpp"
#include "SM2Scheduler.hpp"
#include "Stats.hpp"
#include <ctime>
#include <filesystem>
#include <ncurses.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

static std::string getHomeDirectory() {
  const char *home = getenv("HOME");
  if (!home) {
    struct passwd *pw = getpwuid(getuid());
    home = pw->pw_dir;
  }
  return std::string(home);
}

App::App() : running(true) {
  // Create the deck folder if not exist
  std::string deckDir = getHomeDirectory() + "/.tanki_decks";
  if (!std::filesystem::exists(deckDir)) {
    std::filesystem::create_directories(deckDir);
  }

  ui.init();
  loadDecks();
}

App::~App() {
  saveDecks();
  ui.shutdown();
}

void App::run() {
  // Show the welcome screen
  startScreen();
  if (currentDeck) {
    ui.drawMainMenu(currentDeck->name()); // Force redraw of main menu
    wrefresh(stdscr);
  }

  flushinp();

  while (running) {
    if (!currentDeck) {
      // if user didn't pick a deck, or we have no decks, re-run
      startScreen();
      if (!currentDeck && running) {
        // user might create a deck or might have quit
        continue;
      } else if (!running) {
        break;
      }
    }
    ui.drawMainMenu(currentDeck->name());

    // Fix: Always redraw the menu when switching decks
    ui.drawMainMenu(currentDeck->name());
    wrefresh(stdscr); // Ensure the UI is drawn immediately

    int ch = getch();
    switch (ch) {
    case 'q':
      running = false;
      break;
    case 'r':
      review();
      break;
    case 'c':
      cram();
      break;
    case 'b':
      ui.browseDeck(currentDeck);
      break;
    case 'i':
      importCSV();
      break;
    case 'x':
      deleteCard();
      break;
    case 't':
      showStats();
      break;
    case 's':
      showSchedule();
      break;
    case 'd':
      switchDeck();
      break;
    case 'n':
      createDeck();
      break;
    case 'e':
      examDrillMode();
      break;
    case '?':
      helpScreen();
      break;
    default:
      break;
    }
  }
}

void App::loadDecks() {
  std::string deckDir = getHomeDirectory() + "/.tanki_decks";
  auto files = FileManager::listDeckFiles(deckDir);
  for (auto &f : files) {
    auto deck = FileManager::loadDeck(f);
    if (deck) {
      allDecks.push_back(deck);
    }
  }
}

void App::saveDecks() {
  std::string deckDir = getHomeDirectory() + "/.tanki_decks";
  for (auto &d : allDecks) {
    FileManager::saveDeck(d, deckDir);
  }
}

/**
 * The fancy welcome screen with the Tanki logo, listing all decks,
 * and letting user choose a deck, create a new deck, or quit.
 */
void App::startScreen() {
  if (allDecks.empty()) {
    // If no decks, show simpler screen: create or quit
    int choice = ui.startScreenNoDecks();
    if (choice == 1) {
      createDeck();
    } else {
      running = false;
    }
    return;
  }

  // If decks exist, show them with the logo
  int idx = ui.startScreenWithDecks(allDecks);
  if (idx == -1) {
    // user pressed q => quit
    running = false;
  } else if (idx == -2) {
    // user chose create
    createDeck();
  } else {
    // user picked existing deck
    if (idx >= 0 && idx < (int)allDecks.size()) {
      currentDeck = allDecks[idx];
    }
  }
}

void App::createDeck() {
  std::string name = ui.promptString("Enter a name for the new deck:");
  if (name.empty()) {
    ui.showMessage("No name provided.");
    return;
  }
  // Check duplicates
  for (auto &d : allDecks) {
    if (d->name() == name) {
      ui.showMessage("A deck with that name already exists.");
      return;
    }
  }
  auto deck = std::make_shared<Deck>(name);
  allDecks.push_back(deck);
  currentDeck = deck;
  saveDecks();
  ui.showMessage("Created new deck: " + name);
}

void App::switchDeck() {
  if (allDecks.empty()) {
    ui.showMessage("No decks available.");
    return;
  }
  int idx = ui.showDeckSelection(allDecks);
  if (idx >= 0 && idx < (int)allDecks.size()) {
    currentDeck = allDecks[idx];
    ui.showMessage("Switched to deck: " + currentDeck->name());
  }
}

void App::mergeDecks() { ui.showMessage("Merging not implemented."); }
void App::splitDeck() { ui.showMessage("Splitting not implemented."); }

void App::importCSV() {
  if (!currentDeck) {
    ui.showMessage("No deck selected to import into!");
    return;
  }
  std::string path = ui.promptString("Enter CSV file path:");
  if (path.empty()) {
    ui.showMessage("No path given.");
    return;
  }
  bool ok = FileManager::importCSV(currentDeck, path);
  if (ok)
    ui.showMessage("Import successful!");
  else
    ui.showMessage("Failed to import (file missing or invalid).");
}

void App::exportCSV() { ui.showMessage("Export CSV is not implemented."); }

void App::review() {
  if (!currentDeck) {
    ui.showMessage("No deck selected.");
    return;
  }
  auto dueCards = currentDeck->getDueCards();
  SM2Scheduler sched;
  for (auto &card : dueCards) {
    bool cont = ui.reviewCard(card, false);
    if (!cont)
      break;
    int rating = card.lastRating();
    sched.updateCard(card, rating);
    currentDeck->updateCard(card);
  }
  ui.showMessage("Review session complete.");
}

void App::cram() {
  if (!currentDeck) {
    ui.showMessage("No deck selected.");
    return;
  }
  std::string tag = ui.promptString("Enter tag to cram (blank=all):");
  auto all = currentDeck->cards();
  std::vector<Card> subset;
  if (tag.empty()) {
    subset = all;
  } else {
    for (auto &c : all) {
      if (c.hasTag(tag)) {
        subset.push_back(c);
      }
    }
  }
  for (auto &card : subset) {
    bool cont = ui.reviewCard(card, true);
    if (!cont)
      break;
  }
  ui.showMessage("Cram session done.");
}

/**
 * The new feature: delete a card from the deck.
 * We'll let the user pick a card by index from a small list or prompt.
 */
void App::deleteCard() {
  if (!currentDeck) {
    ui.showMessage("No deck selected!");
    return;
  }
  auto cards = currentDeck->cards();
  if (cards.empty()) {
    ui.showMessage("Deck is empty, nothing to delete.");
    return;
  }

  // We'll reuse UI's "browse" approach with page up/down,
  // then let user pick an index to delete, or 'q' to cancel
  int indexToDel = ui.promptIndexToDelete(cards, currentDeck->name());
  if (indexToDel < 0 || indexToDel >= (int)cards.size()) {
    ui.showMessage("Delete canceled.");
    return;
  }
  // Actually remove the card
  cards.erase(cards.begin() + indexToDel);
  // We have to update the deck’s internal vector
  // simplest is to set the entire new vector:
  currentDeck->setCards(cards);

  ui.showMessage("Card " + std::to_string(indexToDel) + " deleted.");
}

void App::showStats() {
  if (!currentDeck) {
    ui.showMessage("No deck selected!");
    return;
  }
  auto info = Stats::generateStats(currentDeck);
  ui.showLongText("Stats", info);
}

void App::showSchedule() {
  if (!currentDeck) {
    ui.showMessage("No deck selected!");
    return;
  }
  auto s = Stats::generateScheduleInfo(currentDeck);
  ui.showLongText("Schedule", s);
}

void App::helpScreen() {
  std::string help = "Shortcuts:\n"
                     "  r = Review\n"
                     "  c = Cram\n"
                     "  b = Browse\n"
                     "  i = Import CSV\n"
                     "  x = Delete Card\n"
                     "  t = Stats\n"
                     "  s = Schedule\n"
                     "  d = Switch Deck\n"
                     "  n = Create Deck\n"
                     "  ? = Help\n"
                     "  q = Quit\n";
  ui.showLongText("Help", help);
}

void App::examDrillMode() {
  ui.showMessage("Exam/Drill mode not implemented yet.");
}
