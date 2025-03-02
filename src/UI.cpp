#include "UI.hpp"
#include <algorithm>
#include <ncurses.h>
#include <sstream>

UI::UI() : mainWin(nullptr), statusWin(nullptr) {
  colorMenu = 1;
  colorBorder = 2;
  colorFront = 3;
  colorBack = 4;
  colorNormal = 5;
  colorTitle = 6;
}

UI::~UI() {}

void UI::init() {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);

  if (has_colors()) {
    start_color();
    init_pair(colorMenu, COLOR_GREEN, COLOR_BLACK);
    init_pair(colorBorder, COLOR_YELLOW, COLOR_BLACK);
    init_pair(colorFront, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(colorBack, COLOR_CYAN, COLOR_BLACK);
    init_pair(colorNormal, COLOR_WHITE, COLOR_BLACK);
    init_pair(colorTitle, COLOR_RED, COLOR_BLACK);
  }

  int h, w;
  getmaxyx(stdscr, h, w);
  mainWin = newwin(h - 1, w, 0, 0);
  statusWin = newwin(1, w, h - 1, 0);
  wrefresh(mainWin);
  wrefresh(statusWin);
}

void UI::shutdown() {
  if (mainWin)
    delwin(mainWin);
  if (statusWin)
    delwin(statusWin);
  endwin();
}

/**
 * If decks exist, user sees a big Tanki logo, can pick a deck,
 * create new deck, or quit.
 * returns:
 *   - deck index (>=0)
 *   - -1 if user quits
 *   - -2 if user picks "create"
 */
int UI::startScreenWithDecks(const std::vector<std::shared_ptr<Deck>> &decks) {
  clearAll();
  wattron(mainWin, COLOR_PAIR(colorBorder) | A_BOLD);
  box(mainWin, 0, 0);
  wattroff(mainWin, COLOR_PAIR(colorBorder) | A_BOLD);

  // ASCII logoa
  //
  wattron(mainWin, COLOR_PAIR(colorTitle) | A_BOLD);
  mvwprintw(mainWin, 1, 3, " ______          __    ");
  mvwprintw(mainWin, 2, 3, "/_  __/__ ____  / /__ (_)");
  mvwprintw(mainWin, 3, 3, " / / / _ \\/ _ \\/  '_// / ");
  mvwprintw(mainWin, 4, 3, "/_/  \\_,_/_//_/_/\\_\\/_/  ");
  mvwprintw(mainWin, 6, 3, "Tanki --  A Terminal SRS  ");
  wattroff(mainWin, COLOR_PAIR(colorTitle) | A_BOLD);

  // Show decks
  wattron(mainWin, COLOR_PAIR(colorNormal) | A_BOLD);
  mvwprintw(mainWin, 8, 2, "Select a deck to open:");
  wattroff(mainWin, COLOR_PAIR(colorNormal) | A_BOLD);

  int y = 10;
  for (size_t i = 0; i < decks.size(); i++) {
    wattron(mainWin, COLOR_PAIR(colorMenu));
    mvwprintw(mainWin, y, 2, "[%zu]", i);
    wattroff(mainWin, COLOR_PAIR(colorMenu));
    mvwprintw(mainWin, y, 6, "%s", decks[i]->name().c_str());
    y++;
  }

  wattron(mainWin, COLOR_PAIR(colorMenu));
  mvwprintw(mainWin, y + 1, 2, "[c]");
  wattroff(mainWin, COLOR_PAIR(colorMenu));
  mvwprintw(mainWin, y + 1, 6, "Create New Deck");

  wattron(mainWin, COLOR_PAIR(colorMenu));
  mvwprintw(mainWin, y + 2, 2, "[q]");
  wattroff(mainWin, COLOR_PAIR(colorMenu));
  mvwprintw(mainWin, y + 2, 6, "Quit");

  wrefresh(mainWin);
  drawStatusLine("Press deck number, 'c' for create, or 'q' to quit.");

  while (true) {
    int ch = wgetch(mainWin);
    if (ch == 'q') {
      return -1;
    } else if (ch == 'c') {
      return -2;
    } else if (isdigit(ch)) {
      int idx = ch - '0';
      if (idx >= 0 && idx < (int)decks.size()) {
        return idx;
      }
    }
  }
}

/**
 * If no decks exist, ask to create or quit.
 * returns 1 for create, 0 for quit
 */
int UI::startScreenNoDecks() {
  clearAll();
  wattron(mainWin, COLOR_PAIR(colorBorder) | A_BOLD);
  box(mainWin, 0, 0);
  wattroff(mainWin, COLOR_PAIR(colorBorder) | A_BOLD);

  wattron(mainWin, COLOR_PAIR(colorTitle) | A_BOLD);
  mvwprintw(mainWin, 2, 4, "No decks found!");
  wattroff(mainWin, COLOR_PAIR(colorTitle) | A_BOLD);

  mvwprintw(mainWin, 4, 4, "Press ");
  wattron(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
  waddstr(mainWin, "c");
  wattroff(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
  waddstr(mainWin, " to create a new deck, or ");

  wattron(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
  waddstr(mainWin, "q");
  wattroff(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
  waddstr(mainWin, " to quit.");

  wrefresh(mainWin);

  while (true) {
    int ch = wgetch(mainWin);
    if (ch == 'q') {
      return 0;
    } else if (ch == 'c') {
      return 1;
    }
  }
}

void UI::drawMainMenu(const std::string &deckName) {
  clearAll();
  wattron(mainWin, COLOR_PAIR(colorBorder) | A_BOLD);
  box(mainWin, 0, 0);
  wattroff(mainWin, COLOR_PAIR(colorBorder) | A_BOLD);

  wattron(mainWin, COLOR_PAIR(colorTitle) | A_BOLD);
  mvwprintw(mainWin, 0, 2, " TANKI MAIN MENU ");
  wattroff(mainWin, COLOR_PAIR(colorTitle) | A_BOLD);

  wattron(mainWin, COLOR_PAIR(colorNormal));
  mvwprintw(mainWin, 1, 2, "Current Deck: %s", deckName.c_str());
  wattroff(mainWin, COLOR_PAIR(colorNormal));

  if (deckName == "No Deck Selected") {
    mvwprintw(mainWin, 3, 2,
              "No deck selected. Press 'n' to create a new deck!");
  } else {
    mvwprintw(mainWin, 3, 2, "Shortcuts:");

    wattron(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
    mvwprintw(mainWin, 5, 4, "[r]");
    wattroff(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
    mvwprintw(mainWin, 5, 8, "Review due cards");

    wattron(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
    mvwprintw(mainWin, 6, 4, "[c]");
    wattroff(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
    mvwprintw(mainWin, 6, 8, "Cram");

    wattron(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
    mvwprintw(mainWin, 7, 4, "[b]");
    wattroff(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
    mvwprintw(mainWin, 7, 8, "Browse cards");

    wattron(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
    mvwprintw(mainWin, 8, 4, "[i]");
    wattroff(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
    mvwprintw(mainWin, 8, 8, "Import CSV");

    wattron(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
    mvwprintw(mainWin, 9, 4, "[x]");
    wattroff(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
    mvwprintw(mainWin, 9, 8, "Delete card");

    wattron(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
    mvwprintw(mainWin, 10, 4, "[t]");
    wattroff(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
    mvwprintw(mainWin, 10, 8, "Stats");

    wattron(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
    mvwprintw(mainWin, 11, 4, "[s]");
    wattroff(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
    mvwprintw(mainWin, 11, 8, "Schedule");

    wattron(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
    mvwprintw(mainWin, 12, 4, "[d]");
    wattroff(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
    mvwprintw(mainWin, 12, 8, "Switch deck");
  }

  wattron(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
  mvwprintw(mainWin, 14, 4, "[n]");
  wattroff(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
  mvwprintw(mainWin, 14, 8, "Create deck");

  wattron(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
  mvwprintw(mainWin, 15, 4, "[?]");
  wattroff(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
  mvwprintw(mainWin, 15, 8, "Help");

  wattron(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
  mvwprintw(mainWin, 16, 4, "[q]");
  wattroff(mainWin, COLOR_PAIR(colorMenu) | A_BOLD);
  mvwprintw(mainWin, 16, 8, "Quit");

  wrefresh(mainWin);
  drawStatusLine("Ready.");
}

std::string UI::promptString(const std::string &message) {
  echo();
  curs_set(1);

  clearAll();
  wattron(mainWin, COLOR_PAIR(colorBorder) | A_BOLD);
  box(mainWin, 0, 0);
  wattroff(mainWin, COLOR_PAIR(colorBorder) | A_BOLD);

  wattron(mainWin, COLOR_PAIR(colorTitle) | A_BOLD);
  mvwprintw(mainWin, 0, 2, " INPUT ");
  wattroff(mainWin, COLOR_PAIR(colorTitle) | A_BOLD);

  wattron(mainWin, COLOR_PAIR(colorNormal));
  mvwprintw(mainWin, 2, 2, "%s", message.c_str());
  wattroff(mainWin, COLOR_PAIR(colorNormal));

  mvwprintw(mainWin, 4, 2, "> ");
  wmove(mainWin, 4, 4);
  wrefresh(mainWin);

  char buffer[512];
  wgetnstr(mainWin, buffer, 511);

  noecho();
  curs_set(0);
  return std::string(buffer);
}

void UI::browseDeck(std::shared_ptr<Deck> deck) {
  if (!deck) {
    showMessage("No deck to browse!");
    return;
  }

  auto cards = deck->cards();
  const int PAGE_SIZE = 10;
  int total = (int)cards.size();
  int page = 0;

  while (true) {
    clearAll();
    wattron(mainWin, COLOR_PAIR(colorBorder) | A_BOLD);
    box(mainWin, 0, 0);
    wattroff(mainWin, COLOR_PAIR(colorBorder) | A_BOLD);

    wattron(mainWin, COLOR_PAIR(colorTitle) | A_BOLD);
    mvwprintw(mainWin, 0, 2, " BROWSE ");
    wattroff(mainWin, COLOR_PAIR(colorTitle) | A_BOLD);

    mvwprintw(mainWin, 1, 2, "Deck: %s (%d cards)", deck->name().c_str(),
              total);

    int start = page * PAGE_SIZE;
    int end = std::min(start + PAGE_SIZE, total);
    int y = 3;
    for (int i = start; i < end; i++) {
      wattron(mainWin, COLOR_PAIR(colorMenu));
      mvwprintw(mainWin, y, 2, "[%d]", i);
      wattroff(mainWin, COLOR_PAIR(colorMenu));

      // short preview of front
      std::string front = cards[i].front();
      if (front.size() > 50) {
        front = front.substr(0, 47) + "...";
      }
      wattron(mainWin, COLOR_PAIR(colorFront));
      mvwprintw(mainWin, y, 7, "%s", front.c_str());
      wattroff(mainWin, COLOR_PAIR(colorFront));
      y++;
    }

    mvwprintw(mainWin, PAGE_SIZE + 5, 2, "Page %d/%d (n=Next, p=Prev, q=Quit)",
              page + 1, (total + PAGE_SIZE - 1) / PAGE_SIZE);
    wrefresh(mainWin);

    int c = wgetch(mainWin);
    if (c == 'q') {
      break;
    } else if (c == 'n') {
      if (end < total)
        page++;
    } else if (c == 'p') {
      if (page > 0)
        page--;
    }
  }
}

/**
 * Prompt user to pick a card index to delete.
 * We show a small list of cards (PAGE_SIZE=10).
 * Return the chosen index, or -1 if canceled.
 */
int UI::promptIndexToDelete(const std::vector<Card> &cards,
                            const std::string &deckName) {
  if (cards.empty())
    return -1;
  const int PAGE_SIZE = 10;
  int total = (int)cards.size();
  int page = 0;

  while (true) {
    clearAll();
    wattron(mainWin, COLOR_PAIR(colorBorder) | A_BOLD);
    box(mainWin, 0, 0);
    wattroff(mainWin, COLOR_PAIR(colorBorder) | A_BOLD);

    wattron(mainWin, COLOR_PAIR(colorTitle) | A_BOLD);
    mvwprintw(mainWin, 0, 2, " DELETE CARD ");
    wattroff(mainWin, COLOR_PAIR(colorTitle) | A_BOLD);

    mvwprintw(mainWin, 1, 2, "Deck: %s", deckName.c_str());

    int start = page * PAGE_SIZE;
    int end = std::min(start + PAGE_SIZE, total);
    int y = 3;
    for (int i = start; i < end; i++) {
      wattron(mainWin, COLOR_PAIR(colorMenu));
      mvwprintw(mainWin, y, 2, "[%d]", i);
      wattroff(mainWin, COLOR_PAIR(colorMenu));

      std::string front = cards[i].front();
      if (front.size() > 50) {
        front = front.substr(0, 47) + "...";
      }
      wattron(mainWin, COLOR_PAIR(colorFront));
      mvwprintw(mainWin, y, 7, "%s", front.c_str());
      wattroff(mainWin, COLOR_PAIR(colorFront));
      y++;
    }

    mvwprintw(
        mainWin, PAGE_SIZE + 5, 2,
        "Page %d/%d - Enter index to delete, 'n'=Next, 'p'=Prev, 'q'=Cancel",
        page + 1, (total + PAGE_SIZE - 1) / PAGE_SIZE);
    wrefresh(mainWin);

    int c = wgetch(mainWin);
    if (c == 'q') {
      return -1; // canceled
    } else if (c == 'n') {
      if (end < total)
        page++;
    } else if (c == 'p') {
      if (page > 0)
        page--;
    } else if (isdigit(c)) {
      // prompt might only do single digit indexes up to 9
      int idx = c - '0';
      if (idx >= 0 && idx < (int)cards.size()) {
        return idx;
      }
    }
  }
}

int UI::showDeckSelection(const std::vector<std::shared_ptr<Deck>> &decks) {
  clearAll();
  wattron(mainWin, COLOR_PAIR(colorBorder) | A_BOLD);
  box(mainWin, 0, 0);
  wattroff(mainWin, COLOR_PAIR(colorBorder) | A_BOLD);

  wattron(mainWin, COLOR_PAIR(colorTitle) | A_BOLD);
  mvwprintw(mainWin, 0, 2, " SWITCH DECK ");
  wattroff(mainWin, COLOR_PAIR(colorTitle) | A_BOLD);

  for (size_t i = 0; i < decks.size(); i++) {
    wattron(mainWin, COLOR_PAIR(colorMenu));
    mvwprintw(mainWin, i + 2, 2, "[%zu]", i);
    wattroff(mainWin, COLOR_PAIR(colorMenu));

    mvwprintw(mainWin, i + 2, 6, "%s", decks[i]->name().c_str());
  }
  mvwprintw(mainWin, decks.size() + 3, 2, "Enter number or 'q' to cancel:");
  wrefresh(mainWin);

  while (true) {
    int ch = wgetch(mainWin);
    if (ch == 'q') {
      return -1;
    }
    if (isdigit(ch)) {
      int idx = ch - '0';
      if (idx >= 0 && idx < (int)decks.size()) {
        return idx;
      }
    }
  }
}

void UI::showMessage(const std::string &message) {
  clearAll();
  wattron(mainWin, COLOR_PAIR(colorBorder) | A_BOLD);
  box(mainWin, 0, 0);
  wattroff(mainWin, COLOR_PAIR(colorBorder) | A_BOLD);

  wattron(mainWin, COLOR_PAIR(colorTitle) | A_BOLD);
  mvwprintw(mainWin, 0, 2, " MESSAGE ");
  wattroff(mainWin, COLOR_PAIR(colorTitle) | A_BOLD);

  mvwprintw(mainWin, 2, 2, "%s", message.c_str());
  mvwprintw(mainWin, 4, 2, "[Press any key]");
  wrefresh(mainWin);
  wgetch(mainWin);
}

void UI::showLongText(const std::string &title, const std::string &content) {
  clearAll();
  wattron(mainWin, COLOR_PAIR(colorBorder) | A_BOLD);
  box(mainWin, 0, 0);
  wattroff(mainWin, COLOR_PAIR(colorBorder) | A_BOLD);

  wattron(mainWin, COLOR_PAIR(colorTitle) | A_BOLD);
  mvwprintw(mainWin, 0, 2, " %s ", title.c_str());
  wattroff(mainWin, COLOR_PAIR(colorTitle) | A_BOLD);

  int maxy, maxx;
  getmaxyx(mainWin, maxy, maxx);
  int row = 2;
  int col = 2;

  std::istringstream iss(content);
  std::string word;
  while (iss >> word) {
    if (col + (int)word.size() + 1 >= maxx - 2) {
      row++;
      col = 2;
    }
    if (row >= maxy - 2) {
      mvwprintw(mainWin, maxy - 1, 2, "[Press any key]");
      wrefresh(mainWin);
      wgetch(mainWin);

      clearAll();
      wattron(mainWin, COLOR_PAIR(colorBorder) | A_BOLD);
      box(mainWin, 0, 0);
      wattroff(mainWin, COLOR_PAIR(colorBorder) | A_BOLD);

      wattron(mainWin, COLOR_PAIR(colorTitle) | A_BOLD);
      mvwprintw(mainWin, 0, 2, " %s ", title.c_str());
      wattroff(mainWin, COLOR_PAIR(colorTitle) | A_BOLD);

      row = 2;
      col = 2;
    }
    mvwprintw(mainWin, row, col, "%s ", word.c_str());
    col += word.size() + 1;
  }

  mvwprintw(mainWin, row + 2, 2, "[Press any key to continue]");
  wrefresh(mainWin);
  wgetch(mainWin);
}

bool UI::reviewCard(Card &card, bool isCram) {
  clearAll();
  wattron(mainWin, COLOR_PAIR(colorBorder) | A_BOLD);
  box(mainWin, 0, 0);
  wattroff(mainWin, COLOR_PAIR(colorBorder) | A_BOLD);

  wattron(mainWin, COLOR_PAIR(colorTitle) | A_BOLD);
  mvwprintw(mainWin, 0, 2, " %s ", isCram ? "CRAM" : "REVIEW");
  wattroff(mainWin, COLOR_PAIR(colorTitle) | A_BOLD);

  wattron(mainWin, COLOR_PAIR(colorMenu));
  mvwprintw(mainWin, 2, 2, "Front:");
  wattroff(mainWin, COLOR_PAIR(colorMenu));

  wattron(mainWin, COLOR_PAIR(colorFront) | A_BOLD);
  mvwprintw(mainWin, 3, 4, "%s", card.front().c_str());
  wattroff(mainWin, COLOR_PAIR(colorFront) | A_BOLD);

  mvwprintw(mainWin, 5, 2, "[Press any key to flip or 'q' to quit]");
  wrefresh(mainWin);

  int c = wgetch(mainWin);
  if (c == 'q') {
    return false;
  }

  // Show back
  clearAll();
  wattron(mainWin, COLOR_PAIR(colorBorder) | A_BOLD);
  box(mainWin, 0, 0);
  wattroff(mainWin, COLOR_PAIR(colorBorder) | A_BOLD);

  wattron(mainWin, COLOR_PAIR(colorTitle) | A_BOLD);
  mvwprintw(mainWin, 0, 2, " %s ", isCram ? "CRAM" : "REVIEW");
  wattroff(mainWin, COLOR_PAIR(colorTitle) | A_BOLD);

  wattron(mainWin, COLOR_PAIR(colorMenu));
  mvwprintw(mainWin, 2, 2, "Back:");
  wattroff(mainWin, COLOR_PAIR(colorMenu));

  wattron(mainWin, COLOR_PAIR(colorBack) | A_BOLD);
  mvwprintw(mainWin, 3, 4, "%s", card.back().c_str());
  wattroff(mainWin, COLOR_PAIR(colorBack) | A_BOLD);

  mvwprintw(mainWin, 5, 2, "Rate: 1=Again, 2=Hard, 3=Good, 4=Easy, q=quit");
  wrefresh(mainWin);

  while (true) {
    int rc = wgetch(mainWin);
    if (rc == 'q') {
      return false;
    }
    if (rc >= '1' && rc <= '4') {
      card.setLastRating(rc - '0');
      break;
    }
  }
  return true;
}

void UI::drawStatusLine(const std::string &text) {
  werase(statusWin);
  mvwprintw(statusWin, 0, 1, "%s", text.c_str());
  wrefresh(statusWin);
}

void UI::clearAll() {
  werase(mainWin);
  wrefresh(mainWin);
}

void UI::drawBoxTitle(WINDOW *win, const std::string &title) {
  wattron(win, COLOR_PAIR(colorBorder));
  box(win, 0, 0);
  wattroff(win, COLOR_PAIR(colorBorder));
  mvwprintw(win, 0, 2, " %s ", title.c_str());
  wrefresh(win);
}
