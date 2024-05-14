#pragma once

#include <iostream>
#include <list>
#include <vector>
#include <algorithm>

namespace util {
	template <class Key>
	struct hash {
		// hash functor primary template
		static size_t Hash(const Key&) noexcept {
			return size_t();
		}
	};

	template<>
	struct hash<int> {
		[[ nodiscard ]]
		size_t operator()(const float key) const noexcept {
			return key * key;
		}
	};
}

namespace sparkle {
	template<typename Key, typename Value, typename Hasher = util::hash<Key>, typename Alloc = std::allocator<std::pair<const Key, Value>> >
	class unordered_map {
		using value_type = std::pair<const Key, Value>;

		struct node_t {
			value_type kv;
			size_t cached;

			bool operator== (const Key& key) const {
				return kv.first == key;
			}

			friend std::ostream& operator<<(std::ostream& os, const node_t node);
		};

		friend std::ostream& operator<<(std::ostream& os, const node_t node) {
			auto [key, value] = node.kv;

			os
				<< "\nkey: " << key
				<< "\nvalue: " << value
				<< "\ncached key: " << node.cached
				<< "\n-----------------------------\n";

			return os;
		}

		using traits_t = std::allocator_traits<Alloc>;
		using AllocNode = typename traits_t::template rebind_alloc<node_t>;

		using table_t = std::list<node_t, AllocNode>;
		using iterator_t = typename table_t::iterator;

	public:
		auto insert(const Key& key, const Value& value) {
			double load_factor_ = static_cast<double>(elements_.size()) / buckets_.size();
			if (load_factor_ == max_load_factor_) rehash();

			auto cached_key = Hasher{}(key) % buckets_.size();

			auto it = elements_.end();

			if (buckets_[cached_key] == elements_.end()) {
				elements_.emplace_front(std::make_pair(key, value), cached_key);
				it = elements_.begin();
			}
			else
				if (unique(key, cached_key))
					it = elements_.emplace(buckets_[cached_key], std::make_pair(key, value), cached_key);

			buckets_[cached_key] = it;

			return it;
		}

		Value& operator[](const Key& key) {
			auto cached_key = Hasher{}(key) % buckets_.size();
			auto it = std::find(buckets_[cached_key], elements_.end(), key);
			return it->kv.second;
		}

	public:
		void erase(const Key& key) {
			auto cached_key = Hasher{}(key) % buckets_.size();

			auto it = std::find(buckets_[cached_key], elements_.end(), key);

			auto next = it;
			++next;

			elements_.erase(it);

			if (next == elements_.end() || next->cached != cached_key)
				buckets_[cached_key] = elements_.end();
			else
				buckets_[cached_key] = next;
		}

		void print_table() const {
			auto print = [](auto&& a) { std::cout << a; };
			std::ranges::for_each(elements_, print);
		}

	public:
		void set_max_load_factor(double value) {
			max_load_factor_ = value;
		}

		double load_factor() const {
			return static_cast<double>(elements_.size()) / buckets_.size();
		}

		size_t bucket_count() const {
			return 1;
		}

	private:
		void rehash() {
			buckets_.resize(buckets_.size() * buckets_.size());
			for (auto& bucket : buckets_) bucket = elements_.end();

			for (auto it = elements_.begin(), end = elements_.end(); it != end; ++it) {
				auto cached_key = Hasher{}(it->kv.first) % buckets_.size();
				auto old_cached_key = it->cached;

				if (buckets_[cached_key] == elements_.end()) {
					auto new_it = find_by_cached_key(old_cached_key);
					buckets_[cached_key] = new_it;
				}
				it->cached = cached_key;
			}
		}

		bool unique(const Key& key, const Key& cached) {
			return std::find(buckets_[cached], elements_.end(), key) == elements_.end();
		}

		auto find_by_cached_key(const Key& cached_key) {
			for (auto it = elements_.begin(), end = elements_.end(); it != end; ++it)
				if (it->cached == cached_key) return it;
			return elements_.end();
		}

	private:
		double max_load_factor_ = 1.0;

		table_t elements_;
		std::vector<iterator_t> buckets_{ 10, elements_.end() };
	};
}
