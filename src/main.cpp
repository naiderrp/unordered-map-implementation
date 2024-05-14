#include "custom_unordered_map.hpp"

struct test {
	static void insert() {
		sparkle::unordered_map<int, std::string> mapa;

		mapa.insert(1, "1 1");
		mapa.insert(2, "2 2");
		mapa.insert(11, "11 11");
		mapa.insert(22, "22 22");

		mapa.print_table();
	}

	static void erase() {
		sparkle::unordered_map<int, std::string> mapa;

		mapa.insert(1, "1 1");
		mapa.insert(2, "2 2");
		mapa.insert(11, "11 11");
		mapa.insert(22, "22 22");

		mapa.print_table();

		std::cout << "\n============================================\n";

		mapa.erase(2);

		mapa.print_table();
	}

	static void operator_brackets() {
		sparkle::unordered_map<int, std::string> mapa;

		mapa.insert(1, "1 1");
		
		mapa.print_table();

		std::cout << "\n============================================\n";

		mapa[1] = "abc";

		mapa.print_table();
	}

	static void rehash() {

		struct no_action_hash {
			_NODISCARD
			size_t operator()(const int key) const noexcept {
				return key;
			}
		};

		sparkle::unordered_map<size_t, std::string, no_action_hash> mapa;

		mapa.insert(11, "11 11");
		mapa.insert(22, "22 22");
		mapa.insert(2, "2 2");
		mapa.insert(33, "33 33");

		mapa.print_table();

		std::cout << "\n============================================\n";

		mapa.set_max_load_factor(mapa.load_factor());
		
		mapa.insert(3, "3 3");
		
		mapa.print_table();
	}
};

int main() {
	test::insert();
	return 0;
}
