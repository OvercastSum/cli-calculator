/**
 * @file data_processing.cpp
 * @brief Główny plik obliczeniowy (implementacja logiki matematycznej).
 * */

#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <set>
#include <optional>
#include <unordered_map>
#include <map>
#include <stack>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <string.h>

// Tokenize
/**
 * @brief Rozbija ciąg znaków na pojedyncze tokeny matematyczne.
 * * Dzieli tekst na liczby, operatory i zmienne. Dodatkowo wykrywa i obsługuje
 * "minus unarny" (np. w wyrażeniu "-5"), łącząc go z liczbą, zamiast traktować jako odejmowanie.
 * * @param input Surowy tekst równania.
 * @return std::vector<std::string> Lista tokenów gotowa do dalszego przetwarzania.
 */
std::vector<std::string> tokenize(const std::string &input)
{
	std::vector<std::string> result;
	std::regex pattern("([+\\-*/^()=])|([^\\s+\\-*/^()=]+)");

	auto begin = std::sregex_iterator(input.begin(), input.end(), pattern);
	auto end = std::sregex_iterator();

	for (auto match = begin; match != end; ++match)
	{
		result.push_back(match->str());
	}

	std::set<std::string> operators = {"+", "-", "*", "/", "^", "(", ")", "="};
	std::regex numbers("([0-9A-Z]+)|(\\$M[0-9]+)");

	for (int i = result.size() - 1; i >= 0; --i)
	{
		if (result[i] == "-" && i + 1 < result.size())
		{
			bool prev_number_or_variable = (i > 0) && std::regex_match(result[i - 1], numbers);
			bool next_not_operator = !operators.count(result[i + 1]);

			if (!prev_number_or_variable && next_not_operator)
			{
				result[i] = "-" + result[i + 1];
				result.erase(result.begin() + i + 1);
			}
		}
	}

	return result;
}

// Validation
/**
 * @brief Sprawdza, czy operatory są poprawnie rozmieszczone.
 * * Wykrywa błędy takie jak dwa operatory obok siebie (np. "++") lub wyrażenie
 * składające się z samego operatora.
 * * @param tokens Lista tokenów do sprawdzenia.
 * @return true Jeśli operatory są poprawne (lub rzuca wyjątek).
 * @return false Jeśli wyrażenie jest trywialnie błędne (np. sam operator).
 */
bool validateOperators(const std::vector<std::string> &tokens)
{
	std::set<std::string> operators = {"+", "-", "*", "/", "^"};

	if (operators.count(tokens[0]) && tokens.size() == 1)
	{
		return false;
	}

	for (int i = 0; i < tokens.size(); ++i)
	{
		if (i < tokens.size() - 1)
		{
			const auto &current = tokens[i];
			const auto &next = tokens[i + 1];
			if (operators.count(current) && operators.count(next))
			{
				throw std::runtime_error("Dwa operatory obok siebie.");
			}
		}
	}

	return true;
}

/**
 * @brief Weryfikuje poprawność nawiasów w wyrażeniu.
 * * Sprawdza, czy każdy nawias otwierający ma parę oraz czy bilans nawiasów
 * nie schodzi poniżej zera w trakcie analizy.
 * * @param tokens Lista tokenów.
 * @return true Jeśli nawiasy są poprawne.
 * @throws std::runtime_error Jeśli bilans nawiasów jest nieprawidłowy.
 */
bool validateParentheses(const std::vector<std::string> &tokens)
{
	int balance = 0;

	for (int i = 0; i < tokens.size(); i++)
	{
		const auto &token = tokens[i];

		if (token == "(")
		{
			balance++;
		}

		if (token == ")")
		{
			balance--;
			if (balance < 0)
			{
				throw std::runtime_error("Nieprawidlowe nawiasy w rownaniu.");
			}
		}
	}
	if (balance != 0)
	{
		throw std::runtime_error("Niepoprawna liczba nawiasow.");
	}

	return true;
}

/**
 * @brief Upewnia się, że tokeny zawierają tylko dozwolone znaki.
 * * Waliduje format liczb (w tym ułamków), operatorów oraz nazw zmiennych.
 * * @param tokens Lista tokenów.
 * @return true Jeśli wszystkie znaki są dozwolone.
 * @throws std::runtime_error Jeśli napotkano nieznany symbol.
 */
bool allowedCharacter(const std::vector<std::string> &tokens)
{

	std::regex pattern("([-]?[0-9A-Z]+([\\.,][0-9A-Z]+)?|[+\\-*/^()=])|([-]?\\$M[0-9]+)");

	for (int i = 0; i < tokens.size(); i++)
	{
		if (!std::regex_match(tokens[i], pattern))
		{
			throw std::runtime_error("Liczby z niewlasciwego zakresu: " + tokens[i]);
		}
	}
	return true;
}

/**
 * @brief Sprawdza poprawność struktury przypisania do zmiennej.
 * * Wymaga, aby wyrażenie zaczynało się od poprawnej nazwy zmiennej ($M...)
 * i znaku równości.
 * * @param tokens Lista tokenów.
 * @return true Jeśli struktura jest poprawna.
 * @throws std::runtime_error Jeśli brak zmiennej lub znaku równości na początku.
 */
bool validateVariableName(const std::vector<std::string> &tokens)
{
	std::regex pattern("\\$M[0-9]+");
	if (!std::regex_match(tokens[0], pattern))
	{
		throw std::runtime_error("Nieprawidlowa nazwa zmiennej.");
	}

	if (tokens[1] != "=")
	{
		throw std::runtime_error("Brak znaku rownosci na poczatku wyrazenia.");
	}
	return true;
}

/**
 * @brief Agreguje wszystkie testy walidacyjne.
 * * @param tokens Lista tokenów.
 * @return true Jeśli wyrażenie przeszło wszystkie testy pomyślnie.
 */
bool validate(const std::vector<std::string> &tokens)
{
	if (validateOperators(tokens) && validateParentheses(tokens) && allowedCharacter(tokens) && validateVariableName(tokens))
	{
		return true;
	}
	return false;
}

// Normalization
/**
 * @brief Normalizuje format wyrażenia.
 * * Zamienia przecinki na kropki w liczbach oraz wstawia domyślne znaki mnożenia
 * (np. przed nawiasami), aby ułatwić późniejsze parsowanie.
 * * @param tokens Referencja do wektora tokenów (modyfikowana w miejscu).
 * @return std::vector<std::string> Znormalizowany wektor tokenów.
 */
std::vector<std::string> normalize(std::vector<std::string> &tokens)
{
	std::vector<std::string> result;
	std::set<std::string> operators = {"+", "-", "*", "/", "^"};
	std::regex fraction("([-]?[0-9A-Z]+([\\.,][0-9A-Z]+)?)");

	int exponential = 0;
	for (int i = 0; i < tokens.size(); ++i)
	{
		if (tokens[i][0] != '$' && std::regex_match(tokens[i], fraction))
		{
			std::replace(tokens[i].begin(), tokens[i].end(), ',', '.');
		}

		result.push_back(tokens[i]);

		if (exponential == 1)
		{
			result.push_back(")");
			exponential = 0;
		}

		if (i < tokens.size() - 1)
		{
			auto &current = tokens[i];
			const auto &next = tokens[i + 1];

			if (!operators.count(current) && next == "(" && current != "=" && current != "(")
			{
				result.push_back("*");
			}
			else if (current == ")" && next == "(")
			{
				result.push_back("*");
			}
			else if (current == ")" && !operators.count(next) && next != ")")
			{
				result.push_back("*");
			}
			else if (current == "-" && next == "(")
			{
				bool is_unary = operators.count(tokens[i - 1]) || tokens[i - 1] == "(" || tokens[i - 1] == "=";
				if (is_unary)
				{
					result.back() = "-1";
					result.push_back("*");
				}
			}

			else if (current == "^" && next != "(")
			{
				if (i > 0 && tokens[i - 1] == "=")
				{
					result.push_back("(");
					exponential = 1;
				}
			}
		}
	}
	return result;
}

// Parser
/**
 * @brief Parsuje tekst wejściowy, wykonując tokenizację i normalizację.
 * * @param input Surowy tekst równania.
 * @return std::optional<std::vector<std::string>> Wektor przygotowanych tokenów lub std::nullopt w przypadku błędu walidacji.
 */
std::optional<std::vector<std::string>> parser(const std::string &input)
{
	auto tokens = tokenize(input);

	if (!validate(tokens))
	{
		return std::nullopt;
	}

	auto normalized_equation = normalize(tokens);
	return normalized_equation;
}

// ONP
/**
 * @brief Konwertuje wyrażenie na Odwrotną Notację Polską (ONP).
 * * Wykorzystuje algorytm Shunting-yard do uporządkowania operatorów i operandów
 * zgodnie z kolejnością wykonywania działań.
 * * @param input Równanie w formie tekstu.
 * @return std::optional<std::vector<std::string>> Wyrażenie w formacie ONP lub std::nullopt w razie błędu.
 */
std::optional<std::vector<std::string>> ONPConversion(const std::string &input)
{
	auto normalized_equation = parser(input);
	std::vector<std::string> result = {};
	std::stack<std::string> stack = {};

	if (!normalized_equation.has_value())
	{
		return std::nullopt;
	}

	std::map<std::string, int> operator_order = {
		{"+", 1},
		{"-", 1},
		{"*", 2},
		{"/", 2},
		{"^", 3},
	};

	auto tokens = normalized_equation.value();
	for (const auto &token : tokens)
	{
		if (!operator_order.count(token) && token != "(" && token != ")")
		{
			result.push_back(token);
			continue;
		}

		if (token == "(")
		{
			stack.push(token);
			continue;
		}

		if (token == ")")
		{
			while (!stack.empty() && stack.top() != "(")
			{
				result.push_back(stack.top());
				stack.pop();
			}
			if (!stack.empty() && stack.top() == "(")
			{
				stack.pop();
			}
			continue;
		}

		if (operator_order.count(token))
		{
			while (!stack.empty() && stack.top() != "(" && operator_order[token] <= operator_order[stack.top()])
			{
				result.push_back(stack.top());
				stack.pop();
			}
			stack.push(token);
		}
	}

	while (!stack.empty())
	{
		if (stack.top() == "(" || stack.top() == ")")
		{
			return std::nullopt;
		}
		result.push_back(stack.top());
		stack.pop();
	}

	return result;
}

/**
 * @brief Pomocnicza funkcja konwertująca znak na cyfrę.
 * Obsługuje cyfry 0-9 oraz litery A-Z (dla systemów o podstawie > 10).
 */
int charToDigit(char c)
{
	if (std::isdigit(c))
	{
		return c - '0';
	}
	return std::toupper(c) - 'A' + 10;
}

/**
 * @brief Pomocnicza funkcja konwertująca cyfrę na znak.
 * Odwrotność charToDigit.
 */
char digitToChar(int digit)
{
	if (digit < 10)
	{
		return '0' + digit;
	}
	return 'A' + (digit - 10);
}

/**
 * @brief Konwertuje liczbę zapisaną jako tekst w dowolnym systemie na system dziesiętny.
 * * @param number Liczba w formie tekstu (np. "1A").
 * @param base Podstawa systemu liczbowego (2-36).
 * @return std::string Wynik jako tekst reprezentujący liczbę dziesiętną.
 */
std::string anyToDecimal(const std::string &number, int base)
{
	if (base > 36 || base <= 1)
	{
		throw std::out_of_range("Niepoprawny przedzial podstawy systemu liczbowego(2-36)");
	}

	size_t dot_position = number.find('.');
	double result = 0.0;

	size_t int_end = (dot_position != std::string::npos) ? dot_position : number.length();

	double power = 1.0;
	for (int i = int_end - 1; i >= 0; --i)
	{
		int digit = charToDigit(number[i]);
		if (digit >= base)
		{
			throw std::out_of_range("Liczby niezgodne z podanym systemem liczbowym: " + std::to_string(digit));
		}

		result += digit * power;
		power *= base;
	}

	if (dot_position != std::string::npos && dot_position + 1 < number.length())
	{
		power = 1.0 / base;

		for (size_t i = dot_position + 1; i < number.length(); ++i)
		{
			int digit = charToDigit(number[i]);

			if (digit >= base)
			{
				throw std::out_of_range("Liczby niezgodne z podanym systemem liczbowym: " + std::to_string(digit));
			}

			result += digit * power;
			power /= base;
		}
	}
	return std::to_string(result);
}

/**
 * @brief Konwertuje liczbę dziesiętną (double) na tekst w docelowym systemie liczbowym.
 * * @param number Liczba dziesiętna.
 * @param base Docelowa podstawa systemu (2-36).
 * @return std::string Reprezentacja liczby w nowym systemie.
 */
std::string decimalToAny(double number, int base)
{
	if (base > 36 || base < 2)
	{
		throw std::out_of_range("Niepoprawny przedzial podstawy systemu liczbowego (2-36)");
	}

	if (base == 10)
	{
		return std::to_string(number);
	}

	bool is_negative = number < 0;
	if (is_negative)
	{
		number = -number;
	}

	long long int_part = static_cast<long long>(number);
	double fraction_part = number - int_part;

	std::string int_result = "";
	if (int_part == 0)
	{
		int_result = "0";
	}
	else
	{
		while (int_part > 0)
		{
			int digit = int_part % base;
			int_result = digitToChar(digit) + int_result;
			int_part /= base;
		}
	}

	std::string fracResult = "";
	if (fraction_part > 0)
	{
		fracResult = ".";
		int precision = 10;

		while (fraction_part > 0 && precision > 0)
		{
			fraction_part *= base;
			int digit = static_cast<int>(fraction_part);
			fracResult += digitToChar(digit);
			fraction_part -= digit;
			precision--;
		}
	}

	std::string result = int_result + fracResult;

	if (is_negative)
	{
		result = "-" + result;
	}

	return result;
}

/**
 * @brief Przechodzi przez wektor ONP i konwertuje wszystkie liczby na system dziesiętny.
 * * Jest to krok niezbędny przed wykonaniem obliczeń, ponieważ logika matematyczna
 * operuje na typie double (system dziesiętny).
 * * @param onp_equation Wektor tokenów w ONP.
 * @param base Podstawa systemu, w jakim zapisane są liczby w tokenach.
 * @return std::vector<std::string> Wektor tokenów z liczbami dziesiętnymi.
 */
std::vector<std::string> anyToDecimalConversion(const std::vector<std::string> &onp_equation, const int &base)
{
	if (base == 10)
	{
		return onp_equation;
	}

	std::vector<std::string> result = {};

	for (const auto &token : onp_equation)
	{
		if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^")
		{
			result.push_back(token);
		}
		else if (token[0] == '$' || token[1] == '$')
		{
			result.push_back(token);
		}
		else
		{
			result.push_back(anyToDecimal(token, base));
		}
	}

	return result;
}

// Simple Calculations
/**
 * @brief Wykonuje pojedynczą operację arytmetyczną.
 * * @param number_a Pierwszy operand.
 * @param number_b Drugi operand.
 * @param symbol Operator działania (+, -, *, /, ^).
 * @return double Wynik działania.
 * @throws std::runtime_error W przypadku błędu (np. dzielenie przez zero).
 */
double operations(const double &number_a, const double &number_b, const std::string &symbol)
{
	double result{};
	char symbol_char = symbol[0];
	double number_a_double = number_a;
	double number_b_double = number_b;
	switch (symbol_char)
	{
	case '+':
		result = number_a_double + number_b_double;
		break;
	case '-':
		result = number_a_double - number_b_double;
		break;
	case '*':
		result = number_a_double * number_b_double;
		break;
	case '/':
		if (number_b == 0)
		{
			throw std::runtime_error("Dzielenie przez zero");
		}
		result = number_a / number_b;
		break;
	case '^':
		result = std::pow(number_a, number_b);
		break;
	default:
		throw std::runtime_error("Nieznany operator: " + symbol);
	}
	return result;
}

struct Variable
{
	std::string variable_name;
	std::string variable_value_str;
	double variable_value;
};

// ONPCalc
/**
 * @brief Oblicza wartość wyrażenia zapisanego w ONP.
 * * Przechodzi przez tokeny, odkładając liczby na stos i zdejmując je
 * w momencie napotkania operatora. Podstawia też wartości zmiennych z mapy.
 * * @param input Wektor tokenów ONP.
 * @param values Mapa zmiennych.
 * @return double Wynik obliczeń.
 */
double ONPCalc(const std::vector<std::string> &input, std::unordered_map<std::string, double> &values)
{
	std::vector<double> stack;

	for (const auto &token : input)
	{
		if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^")
		{
			if (stack.size() < 2)
			{
				throw std::runtime_error("Nieprawidlowe wyrazenie ONP - brak operandow dla operatora: " + token);
			}

			double b = stack.back();
			stack.pop_back();
			double a = stack.back();
			stack.pop_back();
			double calculated_value = operations(a, b, token);
			stack.push_back(calculated_value);
		}
		else
		{
			std::string token_temp = token;
			if (token[0] == '$' || token[1] == '$')
			{
				if (token[1] == '$')
				{
					token_temp = std::string(token.begin() + 1, token.end());
					if (values.find(token_temp) != values.end())
					{
						stack.push_back(values.at(token_temp) * (-1));
					}
				}
				else if (values.find(token) != values.end())
				{
					stack.push_back(values.at(token));
				}
				else
				{
					throw std::invalid_argument("Nie podano zmiennej: " + token);
				}
			}
			else
			{
				try
				{
					stack.push_back(std::stod(token));
				}
				catch (const std::invalid_argument &error)
				{
					throw std::invalid_argument("Nie mozna przekonwertowac na liczbe(niepoprawny zapis): " + token);
				}
			}
		}
	}

	if (stack.empty())
	{
		throw std::runtime_error("Pusty stos po obliczeniach");
	}
	if (stack.size() > 1)
	{
		throw std::runtime_error("Nieprawidlowe ONP");
	}

	return stack[0];
}

/**
 * @brief Usuwa zbędne zera końcowe oraz ewentualną kropkę dziesiętną.
 * * Funkcja służy do poprawy estetyki wyświetlania liczb zmiennoprzecinkowych.
 * Redukuje ciągi typu "2.500000" do "2.5", a liczby całkowite zapisane z kropką
 * (np. "12.000000") skracane są do formy całkowitej ("12").
 * * @param number Liczba w formacie tekstowym (zazwyczaj wynik std::to_string).
 * @return std::string Liczba bez nadmiarowych zer i kropki na końcu.
 */
std::string cutZeros(const std::string &number)
{
	std::string result = number;
	size_t pos = number.find_last_not_of('0');
	if (pos != std::string::npos)
	{
		result.erase(pos + 1);
	}
	if (!result.empty() && result.back() == '.')
	{
		result.pop_back();
	}
	return result;
}

std::optional<Variable> Calc(const std::string &input, std::unordered_map<std::string, double> &values, const int &base_input, const int &base_output)
{
	auto onp_equation = ONPConversion(input);
	auto map_of_values = values;

	if (!onp_equation.has_value())
	{
		return std::nullopt;
	}

	std::vector<std::string> onp_equation_vector = *onp_equation;
	std::vector<std::string> onp_equation_for_calculations(onp_equation_vector.begin() + 2, onp_equation_vector.end());

	if (base_input != 10)
	{
		onp_equation_for_calculations = anyToDecimalConversion(onp_equation_for_calculations, base_input);
	}

	Variable var = {"", "", 0};

	std::vector<std::string> onp_equation_variable_part(onp_equation_vector.begin(), onp_equation_vector.begin() + 1);

	var.variable_name = onp_equation_variable_part[0];
	var.variable_value = (ONPCalc(onp_equation_for_calculations, values));

	if (base_output != 10)
	{
		var.variable_value_str = decimalToAny(var.variable_value, base_output);
	}
	else
	{
		var.variable_value_str = cutZeros(std::to_string(var.variable_value));
	}

	return var;
}
