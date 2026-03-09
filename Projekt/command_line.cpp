/**
 * @file command_line.cpp
 * @brief Implementacja obsługi argumentów wiersza poleceń i komunikatów pomocy.
 */
#include <iostream>
#include <string>
#include <filesystem>
#include <optional>

/**
 * @brief Wyświetla instrukcję użytkowania programu (help).
 * * Wypisuje na standardowe wyjście składnię wywołania, opis dostępnych flag
 * oraz format danych oczekiwany w pliku wejściowym.
 * * @param argc Liczba argumentów (nieużywana w funkcji, ale zgodna z sygnaturą).
 * @param argv Tablica argumentów (służy do pobrania nazwy pliku wykonywalnego).
 */
void howToUse(int argc, char *argv[])
{
	std::filesystem::path path_to_file(argv[0]);
	std::string file_name = path_to_file.filename().string();
	std::cout << "Kalkulator\n";
	std::cout << "================================\n";
	std::cout << "SKLADNIA: ";
	std::cout << "./"
			  << file_name
			  << " -i <plik wejsciowy>"
			  << " -o <plik wyjsciowy>"
			  << " -s <podstawa systemu>"
			  << " -S <podstawa systemu>\n";
	std::cout << "WYMAGANE: -i, -o\n";
	std::cout << "================================\n";
	std::cout << "FORMAT DANYCH W PLIKU:\n";
	std::cout << "W pliku wejsciowym wyrazenia nalezy zapisywac po nazwie zmiennej(np $M5 = 2+2).\n";
	std::cout << "Zmienne nazywac mozna w następujacy sposob $M(dowolna liczba).\n";
	std::cout << "Zmiennych juz zadeklarowanych mozna uzywac w innych dzialaniach.";
	std::cout << std::endl;
}

/**
 * @brief Pomocnicza funkcja sprawdzająca, czy podany ciąg znaków jest liczbą całkowitą.
 * * Używana do walidacji argumentów określających systemy liczbowe.
 * * @param s Ciąg znaków do sprawdzenia.
 * @return true Jeśli ciąg składa się wyłącznie z cyfr.
 * @return false Jeśli ciąg zawiera inne znaki.
 */
bool isNumber(const std::string &s)
{
	for (char part : s)
	{
		if (!std::isdigit(part))
		{
			return false;
		}
	}
	return true;
}

/**
 * @brief Typy błędów mogących wystąpić podczas parsowania argumentów.
 */
enum class ErrorType
{
	IncorrectPath,
	IncorrectBasis,
	MissingFiles,
	NoValueAfterFlag,
	FlagMultiplication
};

/**
 * @brief Wypisuje sformatowany komunikat błędu na standardowe wyjście.
 * * @param type Rodzaj błędu (z enum ErrorType).
 * @param flag Opcjonalny ciąg znaków (np. nazwa flagi), który ma zostać doklejony do komunikatu.
 */
void showErrorText(ErrorType type, const std::string &flag = "")
{
	switch (type)
	{
	case ErrorType::IncorrectPath:
		std::cout << "Wskazano niepoprawna sciezke do pliku." << std::endl;
		break;
	case ErrorType::IncorrectBasis:
		std::cout << "Wpisano nieprawidlowa podstawe systemu liczbowego." << std::endl;
		break;
	case ErrorType::MissingFiles:
		std::cout << "Nie wskazano potrzebnych plikow" << std::endl;
		break;
	case ErrorType::NoValueAfterFlag:
		std::cout << "Brak nazwy pliku po " << flag << std::endl;
		break;
	case ErrorType::FlagMultiplication:
		std::cout << "Podano kilkukrotnie ta sama flage" << std::endl;
	}
}

struct Arguments
{
	std::string input_file;
	std::string output_file;
	std::string input_num_system;
	std::string output_num_system;
};

std::optional<Arguments> parseCommand(int argc, char *argv[])
{
	bool is_input_file = false;
	bool is_output_file = false;
	bool is_input_num_system = false;
	bool is_output_num_system = false;
	Arguments args = {"", "", "", ""};

	if (argc == 1)
	{
		howToUse(argc, argv);
		return std::nullopt;
	}

	for (int i = 1; i < argc; i++)
	{
		if (std::string(argv[i]) == "-i")
		{
			if (is_input_file)
			{
				showErrorText(ErrorType::FlagMultiplication);
				return std::nullopt;
			}

			if (i + 1 >= argc || argv[i + 1][0] == '-')
			{
				showErrorText(ErrorType::NoValueAfterFlag, argv[i]);
				return std::nullopt;
			}

			if (!std::filesystem::exists(argv[i + 1]))
			{
				showErrorText(ErrorType::IncorrectPath);
				return std::nullopt;
			}

			is_input_file = true;
			args.input_file = argv[i + 1];
			continue;
		}

		if (std::string(argv[i]) == "-o")
		{
			if (is_output_file)
			{
				showErrorText(ErrorType::FlagMultiplication);
				return std::nullopt;
			}

			if (i + 1 >= argc || argv[i + 1][0] == '-')
			{
				showErrorText(ErrorType::NoValueAfterFlag, argv[i]);
				return std::nullopt;
			}

			is_output_file = true;
			args.output_file = argv[i + 1];
			continue;
		}

		if (std::string(argv[i]) == "-s")
		{
			if (is_input_num_system)
			{
				showErrorText(ErrorType::FlagMultiplication);
				return std::nullopt;
			}

			if (i + 1 >= argc || argv[i + 1][0] == '-')
			{
				showErrorText(ErrorType::NoValueAfterFlag, argv[i]);
				return std::nullopt;
			}

			if (!isNumber(argv[i + 1]))
			{
				showErrorText(ErrorType::IncorrectBasis);
				return std::nullopt;
			}

			is_input_num_system = true;
			args.input_num_system = argv[i + 1];
			continue;
		}

		if (std::string(argv[i]) == "-S")
		{
			if (is_output_num_system)
			{
				showErrorText(ErrorType::FlagMultiplication);
				return std::nullopt;
			}

			if (i + 1 >= argc || argv[i + 1][0] == '-')
			{
				showErrorText(ErrorType::NoValueAfterFlag, argv[i]);
				return std::nullopt;
			}

			if (!isNumber(argv[i + 1]))
			{
				showErrorText(ErrorType::IncorrectBasis);
				return std::nullopt;
			}

			is_output_num_system = true;
			args.output_num_system = argv[i + 1];
			continue;
		}
	}

	if (args.input_file == "" || args.output_file == "")
	{
		showErrorText(ErrorType::MissingFiles);
		exit(1);
	}
	else
	{
		if (args.input_num_system == "")
		{
			args.input_num_system = "10";
		}

		if (args.output_num_system == "")
		{
			args.output_num_system = "10";
		}
	}

	return args;
}
