/**
 * @file file_handling.h
 * @brief Interfejs do odczytu i zapisu plików tekstowych.
 */
#pragma once
#include <optional>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

/**
 * @brief Wczytuje zawartość pliku tekstowego linia po linii.
 * * Funkcja otwiera wskazany plik i przepisuje jego zawartość do wektora,
 * gdzie każdy element to osobna linia (równanie).
 * * @param file_name Ścieżka do pliku wejściowego.
 * @return std::optional<std::vector<std::string>> Wektor zawierający linie z pliku.
 * @throws std::runtime_error Gdy nie uda się otworzyć pliku do odczytu.
 */
std::optional<std::vector<std::string>> readFile(const std::string &file_name);

/**
 * @brief Zapisuje wyniki obliczeń do pliku wyjściowego.
 * * Funkcja najpierw usuwa stary plik (jeśli istnieje), a następnie tworzy nowy
 * i zapisuje w nim pary "Zmienna = Wynik".
 * * @param file_name Ścieżka do pliku wynikowego.
 * @param results Wektor par zawierający nazwę zmiennej oraz jej obliczoną wartość (jako string).
 * @throws std::runtime_error Gdy nie uda się otworzyć pliku do zapisu.
 */
void writeFile(const std::string &file_name, const std::vector<std::pair<std::string, std::string>> &results);