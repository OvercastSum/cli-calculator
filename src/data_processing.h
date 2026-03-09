/**
 * @file data_processing.h
 * @brief Interfejs silnika obliczeniowego oraz definicja struktury zmiennych.
 */
#pragma once
#include <vector>
#include <string>
#include <optional>
#include <unordered_map>

/**
 * @brief Struktura reprezentująca wynik obliczeń przypisany do konkretnej zmiennej.
 */
struct Variable
{
	std::string variable_name;
	std::string variable_value_str;
	double variable_value;
};

/**
 * @brief Główna funkcja kalkulatora. Przetwarza równanie, zamienia systemy liczbowe i wykonuje obliczenia.
 * * Funkcja przyjmuje równanie w formie tekstu, parsuje je, konwertuje na Odwrotną Notację Polską (ONP),
 * a następnie oblicza wynik, uwzględniając podane wartości zmiennych. Obsługuje również konwersję
 * systemów liczbowych na wejściu i wyjściu.
 * * @param input Równanie matematyczne do obliczenia w formie ciągu znaków.
 * @param values Mapa zawierająca wartości zmiennych (klucz to nazwa zmiennej, np. "$M1").
 * @param base_input Podstawa systemu liczbowego, w którym zapisane są liczby w równaniu (np. 10, 16).
 * @param base_output Podstawa systemu liczbowego, w którym ma zostać zwrócony wynik.
 * @return std::optional<Variable> Obiekt zawierający wynik obliczeń lub std::nullopt, jeśli wystąpił błąd parsowania.
 * @throws std::runtime_error W przypadku błędów matematycznych (np. dzielenie przez zero) lub błędów składni.
 * @throws std::invalid_argument Gdy podana zmienna nie istnieje w mapie values.
 * @throws std::out_of_range Gdy podstawa systemu liczbowego jest nieprawidłowa.
 */
std::optional<Variable> Calc(const std::string &input, std::unordered_map<std::string, double> &values, const int &base_input, const int &base_output);
