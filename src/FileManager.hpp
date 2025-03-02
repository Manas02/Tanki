#ifndef TANKI_FILEMANAGER_HPP
#define TANKI_FILEMANAGER_HPP

#include "Deck.hpp"
#include <memory>
#include <string>
#include <vector>

class FileManager {
public:
  static std::vector<std::string> listDeckFiles(const std::string &directory);
  static std::shared_ptr<Deck> loadDeck(const std::string &path);
  static bool saveDeck(std::shared_ptr<Deck> deck,
                       const std::string &directory);

  // CSV import with duplicates check
  static bool importCSV(std::shared_ptr<Deck> deck, const std::string &csvPath);

  // Not implemented
  static bool exportCSV(std::shared_ptr<Deck> deck, const std::string &csvPath);
};

#endif // TANKI_FILEMANAGER_HPP
