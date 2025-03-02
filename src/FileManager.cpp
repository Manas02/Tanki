#include "FileManager.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>

std::vector<std::string>
FileManager::listDeckFiles(const std::string &directory) {
  std::vector<std::string> result;
  for (auto &entry : std::filesystem::directory_iterator(directory)) {
    if (entry.path().extension() == ".deck") {
      result.push_back(entry.path().string());
    }
  }
  return result;
}

std::shared_ptr<Deck> FileManager::loadDeck(const std::string &path) {
  std::ifstream fin(path);
  if (!fin.good())
    return nullptr;

  std::string deckName;
  if (!std::getline(fin, deckName)) {
    return nullptr;
  }
  auto deck = std::make_shared<Deck>(deckName);

  // Each line: front|back|interval|EF|dueDate|suspended|tags
  std::string line;
  while (std::getline(fin, line)) {
    if (line.empty())
      continue;
    std::stringstream ss(line);

    std::string front, back, iStr, efStr, dueStr, suspStr, tags;
    if (!std::getline(ss, front, '|'))
      continue;
    if (!std::getline(ss, back, '|'))
      continue;
    if (!std::getline(ss, iStr, '|'))
      continue;
    if (!std::getline(ss, efStr, '|'))
      continue;
    if (!std::getline(ss, dueStr, '|'))
      continue;
    if (!std::getline(ss, suspStr, '|'))
      continue;
    if (!std::getline(ss, tags, '|'))
      continue;

    Card c(front, back);
    c.setInterval(std::stoi(iStr));
    c.setEaseFactor(std::stod(efStr));
    c.setDueDate(std::stol(dueStr));
    c.setSuspended(suspStr == "1");
    c.setTags(tags);

    deck->addCard(c);
  }
  return deck;
}

bool FileManager::saveDeck(std::shared_ptr<Deck> deck,
                           const std::string &directory) {
  if (!deck)
    return false;
  std::string filename = directory + "/" + deck->name() + ".deck";
  std::ofstream fout(filename);
  if (!fout.is_open())
    return false;

  fout << deck->name() << "\n";
  for (auto &c : deck->cards()) {
    fout << c.front() << "|" << c.back() << "|" << c.interval() << "|"
         << c.easeFactor() << "|" << c.dueDate() << "|"
         << (c.isSuspended() ? "1" : "0") << "|" << c.tagsString() << "|"
         << "\n";
  }
  return true;
}

/**
 * Import CSV with 2 columns: front,back
 * Check duplicates by storing existing "front|back" combos in a set
 */
bool FileManager::importCSV(std::shared_ptr<Deck> deck,
                            const std::string &csvPath) {
  if (!deck)
    return false;

  // Build a set of existing front|back combos
  std::unordered_set<std::string> existing;
  for (auto &card : deck->cards()) {
    existing.insert(card.front() + "|" + card.back());
  }

  std::ifstream fin(csvPath);
  if (!fin.is_open()) {
    return false;
  }

  std::string line;
  while (std::getline(fin, line)) {
    if (line.empty())
      continue;
    std::stringstream ss(line);
    std::string front, back;
    if (!std::getline(ss, front, ','))
      continue;
    if (!std::getline(ss, back, ','))
      continue;

    // remove quotes if present
    if (!front.empty() && front.front() == '"')
      front.erase(front.begin());
    if (!front.empty() && front.back() == '"')
      front.pop_back();
    if (!back.empty() && back.front() == '"')
      back.erase(back.begin());
    if (!back.empty() && back.back() == '"')
      back.pop_back();

    // check duplicate
    std::string combo = front + "|" + back;
    if (existing.find(combo) == existing.end()) {
      // new
      Card c(front, back);
      deck->addCard(c);
      existing.insert(combo);
    }
    // else skip duplicate
  }
  return true;
}

bool FileManager::exportCSV(std::shared_ptr<Deck> deck,
                            const std::string &csvPath) {
  // not implemented
  return false;
}
