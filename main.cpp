#include "string_compressor.h"

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>

using string_compressor::CompressionException;
using string_compressor::StringCompressor;

constexpr int kMinStringLength = 1;
constexpr int kMaxStringLength = 1000;
constexpr int kMaxRandomGroupSize = 5;
constexpr int kGroupGenerationChance = 3;

void ClearTerminal() {
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}

void ClearInputBuffer() {
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool IsExitToMenu(const std::string& input) {
  return (input == "0" || input == "menu" || input == "Menu" || input == "Меню");
}

void PrintSeparator() {
  std::cout << "\n========================================\n";
}

void PrintHeader() {
  std::cout << "\n";
  std::cout << "    ****************************************\n";
  std::cout << "    *     СЖАТИЕ СТРОКИ ПО ПРАВИЛУ        *\n";
  std::cout << "    *  (замена повторов: символ + число)  *\n";
  std::cout << "    ****************************************\n";
  PrintSeparator();
}

void PrintMenu() {
  std::cout << "\nГЛАВНОЕ МЕНЮ:\n";
  std::cout << "  1. Ввод строки с клавиатуры\n";
  std::cout << "  2. Ввод строки из файла\n";
  std::cout << "  3. Генерация случайной строки\n";
  std::cout << "  4. Информация о программе\n";
  std::cout << "  0. Выход\n";
  std::cout << "\nВыберите действие (0-4): ";
}

void PrintCompressionResult(const StringCompressor& compressor) {
  std::string_view input_view = compressor.GetInputString();
  std::cout << "\n--- РЕЗУЛЬТАТ СЖАТИЯ ---" << std::endl;
  std::cout << "Исходная строка: \"" << input_view << "\"" << std::endl;
  std::cout << "Длина исходной: " << input_view.length() << std::endl;

  auto opt_result = compressor.GetCompressedResult();
  if (!opt_result.has_value()) {
    std::cout << "Сжатие ещё не выполнялось." << std::endl;
    return;
  }

  std::string_view result_view = opt_result.value();
  std::cout << "Сжатая строка: \"" << result_view << "\"" << std::endl;
  std::cout << "Длина сжатой: " << result_view.length() << std::endl;

  if (compressor.WasCompressed()) {
    int saved = static_cast<int>(input_view.length() - result_view.length());
    if (saved > 0) {
      std::cout << "Сэкономлено символов: " << saved << std::endl;
      double percent = (static_cast<double>(saved) / input_view.length()) * 100;
      std::cout << "Эффективность сжатия: " << percent << "%" << std::endl;
    } else if (saved < 0) {
      std::cout << "Строка увеличилась на: " << -saved << " символов" << std::endl;
    }
  } else {
    std::cout << "Строка не требует сжатия" << std::endl;
  }
}

bool InputFromKeyboard(StringCompressor* compressor) {
  PrintSeparator();
  std::cout << "ВВОД СТРОКИ С КЛАВИАТУРЫ\n";
  std::cout << "Правила: латинские буквы a-z, A-Z\n";
  std::cout << "(Для выхода в меню введите 0)\n";

  while (true) {
    std::cout << "\nВведите строку: ";
    std::string input;
    std::getline(std::cin, input);

    if (IsExitToMenu(input)) {
      std::cout << "\nВозврат в главное меню..." << std::endl;
      return false;
    }

    size_t start = input.find_first_not_of(" \t\n\r");
    if (start != std::string::npos) input = input.substr(start);
    size_t end = input.find_last_not_of(" \t\n\r");
    if (end != std::string::npos && end + 1 < input.length())
      input = input.substr(0, end + 1);

    if (input.empty()) {
      std::cout << "Ошибка: пустая строка. Попробуйте снова." << std::endl;
      continue;
    }

    compressor->SetInputString(std::move(input));
    try {
      compressor->Compress();
      std::cout << "\nСтрока успешно сжата!" << std::endl;
      PrintCompressionResult(*compressor);
      return true;
    } catch (const CompressionException& e) {
      std::cout << "Ошибка: " << e.what() << " Попробуйте снова." << std::endl;
    }
  }
}

bool InputFromFile(StringCompressor* compressor) {
  PrintSeparator();
  std::cout << "ВВОД СТРОКИ ИЗ ФАЙЛА\n";
  std::cout << "(Для выхода в меню введите 0)\n";

  while (true) {
    std::cout << "\nВведите имя файла: ";
    std::string filename;
    std::getline(std::cin, filename);

    if (IsExitToMenu(filename)) {
      std::cout << "\nВозврат в главное меню..." << std::endl;
      return false;
    }

    if (filename.empty()) {
      std::cout << "Ошибка: имя файла не может быть пустым." << std::endl;
      continue;
    }

    std::ifstream file(filename);
    if (!file.is_open()) {
      std::cout << "Ошибка: не удалось открыть файл \"" << filename << "\"" << std::endl;
      continue;
    }

    std::string content;
    std::getline(file, content);
    file.close();

    if (content.empty()) {
      std::cout << "Ошибка: файл пуст." << std::endl;
      continue;
    }

    size_t start = content.find_first_not_of(" \t\n\r");
    if (start != std::string::npos) content = content.substr(start);
    size_t end = content.find_last_not_of(" \t\n\r");
    if (end != std::string::npos && end + 1 < content.length())
      content = content.substr(0, end + 1);

    if (content.empty()) {
      std::cout << "Ошибка: файл содержит только пробелы." << std::endl;
      continue;
    }

    compressor->SetInputString(std::move(content));
    try {
      compressor->Compress();
      std::cout << "\nСтрока загружена и сжата!" << std::endl;
      std::cout << "Содержимое файла: \"" << compressor->GetInputString() << "\"" << std::endl;
      PrintCompressionResult(*compressor);
      return true;
    } catch (const CompressionException& e) {
      std::cout << "Ошибка: " << e.what() << " Попробуйте снова." << std::endl;
    }
  }
}

bool GenerateRandomString(StringCompressor* compressor) {
  PrintSeparator();
  std::cout << "ГЕНЕРАЦИЯ СЛУЧАЙНОЙ СТРОКИ\n";
  std::cout << "(Для выхода в меню введите 0)\n";

  while (true) {
    std::cout << "\nВведите длину строки (" << kMinStringLength << "-" << kMaxStringLength << "): ";
    std::string input;
    std::getline(std::cin, input);

    if (IsExitToMenu(input)) {
      std::cout << "\nВозврат в главное меню..." << std::endl;
      return false;
    }

    bool is_number = true;
    for (char c : input)
      if (c < '0' || c > '9') { is_number = false; break; }
    if (!is_number || input.empty()) {
      std::cout << "Ошибка: введите целое число." << std::endl;
      continue;
    }

    int length = std::stoi(input);
    if (length < kMinStringLength || length > kMaxStringLength) {
      std::cout << "Ошибка: длина должна быть от " << kMinStringLength
                << " до " << kMaxStringLength << "." << std::endl;
      continue;
    }

    std::srand(static_cast<unsigned>(std::time(nullptr)));
    const std::string letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string random_str;
    random_str.reserve(length);

    for (int i = 0; i < length; ++i) {
      if (std::rand() % kGroupGenerationChance == 0 && i < length - 1) {
        char letter = letters[std::rand() % letters.length()];
        int group_size = std::rand() % kMaxRandomGroupSize + 2;
        if (i + group_size > length) group_size = length - i;
        for (int j = 0; j < group_size; ++j) random_str += letter;
        i += group_size - 1;
      } else {
        random_str += letters[std::rand() % letters.length()];
      }
    }

    compressor->SetInputString(std::move(random_str));
    try {
      compressor->Compress();
      std::cout << "\nСгенерирована строка: \"" << compressor->GetInputString() << "\"" << std::endl;
      PrintCompressionResult(*compressor);
      return true;
    } catch (const CompressionException& e) {
      std::cout << "Ошибка: " << e.what() << std::endl;
    }
  }
}

void ShowInfo() {
  PrintSeparator();
  std::cout << "ИНФОРМАЦИЯ О ПРОГРАММЕ\n";
  std::cout << "\nПравило сжатия: замена повторов (символ + число)\n";
  std::cout << "Пример: 'aaabbb' -> 'a3b3'\n";
  std::cout << "Сжатие повторяется до стабилизации.\n";
  std::cout << "Ограничения: только латинские буквы, длина до 1000.\n";
}

int GetMenuChoice() {
  while (true) {
    std::string input;
    std::getline(std::cin, input);
    bool is_number = true;
    for (char c : input)
      if (c < '0' || c > '9') { is_number = false; break; }
    if (!is_number || input.empty()) {
      std::cout << "Ошибка: введите число от 0 до 4: ";
      continue;
    }
    int choice = std::stoi(input);
    if (choice >= 0 && choice <= 4) return choice;
    std::cout << "Ошибка: введите число от 0 до 4: ";
  }
}

int main() {
  StringCompressor compressor;
  int choice;

  PrintHeader();

  do {
    PrintMenu();
    choice = GetMenuChoice();

    switch (choice) {
      case 1: InputFromKeyboard(&compressor); break;
      case 2: InputFromFile(&compressor); break;
      case 3: GenerateRandomString(&compressor); break;
      case 4: ShowInfo(); break;
      case 0: std::cout << "\nДо свидания!" << std::endl; break;
    }

    if (choice != 0) {
      std::cout << "\nНажмите Enter для продолжения...";
      std::cin.get();
      ClearTerminal();
      PrintHeader();
    }
  } while (choice != 0);

  return 0;
}