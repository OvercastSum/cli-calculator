/**
 * @file command_line.h
 * @brief Definicja struktury argumentów i deklaracje funkcji parsujących flagi startowe.
 */
#pragma once
#include <string>
#include <optional>

/**
 * @brief Struktura przechowywująca konfigurację uruchomieniową programu.
 * * Zawiera ścieżki do plików oraz ustawienia systemów liczbowych pobrane
 * z argumentów wiersza poleceń.
 */
struct Arguments {
	std::string input_file;
	std::string output_file;
	std::string input_num_system;
	std::string output_num_system;
};

/**
 * @brief Analizuje argumenty wiersza poleceń przekazane do programu.
 * * Funkcja weryfikuje poprawność flag, sprawdza istnienie pliku wejściowego
 * oraz waliduje format podanych systemów liczbowych. Jeśli uruchomiono program
 * bez argumentów, wyświetla instrukcję obsługi.
 * * @param argc Liczba argumentów (argument count).
 * @param argv Tablica argumentów (argument vector).
 * @return std::optional<Arguments> Wypełniona struktura z ustawieniami lub std::nullopt w przypadku błędu walidacji.
 */
std::optional<Arguments> parseCommand(int argc, char* argv[]);
