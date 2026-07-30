#pragma once

#include <forward_list>
#include <tuple>
#include <unordered_map>

template <class ...Keys>
struct tupple_hash {

    using key_type = std::tuple<Keys...>;

    size_t operator()(key_type const& key) const {
        size_t hash = 0;
        std::apply([&](auto&&... args) { ((hash += (args * 7919 + args * 6361 + args * 3001 + args)), ...); }, key);
        return hash;
    }
};

template <class Tp, class ...Keys>
class group_list {

    public:
        using value_type = Tp;
        using reference = Tp&;
        using const_reference = const Tp&;
        using iterator = typename std::forward_list<value_type>::iterator;
        using const_iterator = typename std::forward_list<value_type>::const_iterator;
        using difference_type = typename std::forward_list<value_type>::difference_type;
        using size_type = typename std::forward_list<value_type>::size_type;
        

    public:
        inline iterator begin() noexcept { return m_list.begin(); }
        inline iterator end() noexcept { return m_list.end(); }

        inline const_iterator cbegin() const noexcept{ return m_list.cbegin(); }
        inline const_iterator cend() const noexcept { return m_list.cend(); }

        inline bool empty() const noexcept { return m_list.empty(); }
        inline size_type max_size() const noexcept { return m_list.max_size(); }


        inline void swap(std::forward_list<Tp>& other) { m_list.swap(other); }
        inline void clear() noexcept { 
            m_list.clear(); 
            m_indices.clear();
        }
        
        iterator insert(const Tp& val, Keys&&... keys) {

            auto index = m_indices.find(std::make_tuple(keys...));
            if (index != m_indices.cend())
                return m_list.insert_after(index->second, val);

            /* Emplace item and key */
            iterator new_index = m_list.insert_after(m_list.before_begin(), val);
            m_indices[std::make_tuple(keys...)] = new_index;
            return new_index;
        }

        iterator insert(Tp&& val, Keys&&... keys) {
            auto index = m_indices.find(std::make_tuple(keys...));
            if (index != m_indices.end())
                return m_list.insert_after(index->second, val);

            /* Emplace item and key */
            iterator new_index = m_list.insert_after(m_list.before_begin(), val);
            m_indices[std::make_tuple(keys...)] = new_index;
            return new_index;
        }
        
    private:
        std::forward_list<value_type> m_list;
        std::unordered_map<std::tuple<Keys...>, const_iterator, tupple_hash<Keys...>> m_indices;
};
