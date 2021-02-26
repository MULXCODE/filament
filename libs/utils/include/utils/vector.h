/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef UTILS_VECTOR_H
#define UTILS_VECTOR_H

#include <vector>

namespace utils {

/**
 * Inserts the specified item in the vector at its sorted position.
 */
template <typename T>
static inline void insert_sorted(std::vector<T>& v, T item) {
    auto pos = std::lower_bound(v.begin(), v.end(), item);
    v.insert(pos, std::move(item));
}

/**
 * Inserts the specified item in the vector at its sorted position.
 * The item type must implement the < operator. If the specified
 * item is already present in the vector, this method returns without
 * inserting the item again.
 *
 * @return True if the item was inserted at is sorted position, false
 *         if the item already exists in the vector.
 */
template <typename T>
static inline bool insert_sorted_unique(std::vector<T>& v, T item) {
    if (UTILS_LIKELY(v.size() == 0 || v.back() < item)) {
        v.push_back(item);
        return true;
    }

    auto pos = std::lower_bound(v.begin(), v.end(), item);
    if (UTILS_LIKELY(pos == v.end() || item < *pos)) {
        v.insert(pos, std::move(item));
        return true;
    }

    return false;
}

//  -----------------------------------------------------------------------------------------------

class TrivialVectorBase {
protected:
    using size_type = uint32_t;

    char* mBegin = nullptr;
    size_type mItemCount = 0;
    size_type mCapacity = 0;

    TrivialVectorBase() noexcept = default;

    TrivialVectorBase(size_type itemSize, size_type count) noexcept
            : mItemCount(count), mCapacity(count) {
        mBegin = (char*)malloc(mCapacity * itemSize);  // FIXME: move this in cpp
    }

    TrivialVectorBase(TrivialVectorBase&& rhs) noexcept {
        this->swap(rhs);
    }

    ~TrivialVectorBase() noexcept {
        free(mBegin);
    }

    inline void assert_capacity(size_type c, size_type itemSize) {
        if (UTILS_UNLIKELY(mCapacity < c)) {
            assert_capacity_slow(c, itemSize);
        }
    }

    UTILS_NOINLINE
    void assert_capacity_slow(size_type c, size_type itemSize) {
        c = (c * 3 + 1) / 2;
        set_capacity(c, itemSize);
    }

    UTILS_NOINLINE
    void set_capacity(size_type n, size_type itemSize) {
        // FIXME: move this in cpp
        if (n == mCapacity) {
            return;
        }
        char* addr = (char*)malloc(n * itemSize);
        memcpy(addr, mBegin, ((mItemCount < n) ? mItemCount : n) * itemSize);
        free(mBegin);
        mBegin = addr;
        mCapacity = n;
    }

    void swap(TrivialVectorBase& other) {
        using std::swap;
        swap(mBegin, other.mBegin);
        swap(mItemCount, other.mItemCount);
        swap(mCapacity, other.mCapacity);
    }

    char* begin() noexcept { return mBegin; }
    char* end(size_type itemSize) noexcept { return mBegin + mItemCount * itemSize; }
    char const* begin() const noexcept { return const_cast<TrivialVectorBase*>(this)->begin(); }
    char const* end(size_type itemSize) const noexcept { return const_cast<TrivialVectorBase*>(this)->end(itemSize); }
};

template<typename T, std::enable_if_t<std::is_trivial<T>::value, bool> = true>
class vector : private TrivialVectorBase {
public:
    using value_type = T;
    using reference = T&;
    using const_reference = T const&;
    using size_type = TrivialVectorBase::size_type;
    using difference_type = int32_t;
    using pointer = T*;
    using const_pointer = T const*;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;


    vector() noexcept = default;

    explicit vector(size_type count) noexcept : TrivialVectorBase(sizeof(T), count) { }

    explicit vector(size_type count, const value_type& proto) noexcept
            : TrivialVectorBase(sizeof(T), count) {
        std::generate_n(begin(), count, [proto]{ return proto; });
    }

    // --------------------------------------------------------------------------------------------

    iterator begin() noexcept { return reinterpret_cast<iterator>(TrivialVectorBase::begin()); }

    iterator end() noexcept { return reinterpret_cast<iterator>(TrivialVectorBase::end(sizeof(T))); }

    const_iterator begin() const noexcept { return reinterpret_cast<const_iterator>(TrivialVectorBase::begin()); }

    const_iterator end() const noexcept { return reinterpret_cast<const_iterator>(TrivialVectorBase::end(sizeof(T))); }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

    const_iterator cbegin() const noexcept { return begin(); }

    const_iterator cend() const noexcept { return end(); }

    const_reverse_iterator crbegin() const noexcept { return rbegin(); }

    const_reverse_iterator crend() const noexcept { return rend(); }

    // --------------------------------------------------------------------------------------------

    size_type size() const noexcept { return mItemCount; }

    size_type capacity() const noexcept { return mCapacity; }

    bool empty() const noexcept { return mItemCount == 0; }

    // --------------------------------------------------------------------------------------------

    reference operator[](size_type n) noexcept { return *(begin() + n); }

    const_reference operator[](size_type n) const noexcept { return *(begin() + n); }

    reference front() noexcept { return *begin(); }

    const_reference front() const noexcept { return *begin(); }

    reference back() noexcept { return *(end() - 1); }

    const_reference back() const noexcept { return *(end() - 1); }

    value_type* data() noexcept { return begin(); }

    const value_type* data() const noexcept { return begin(); }

    // --------------------------------------------------------------------------------------------

    void push_back(const_reference v) {
        assert_capacity(size() + 1, sizeof(T));
        *end() = v;
        mItemCount++;
    }

    template<typename ... ARGS>
    reference emplace_back(ARGS&& ... args) {
        assert_capacity(size() + 1, sizeof(T));
        T::T(end())(std::forward<ARGS>(args)...);
        mItemCount++;
        return this->back();
    }

    void pop_back() {
        mItemCount--;
    }

    iterator insert(const_iterator position, const_reference v) {
        assert_capacity(size() + 1, sizeof(T));
        std::move_backward(position, end(), end() + 1);
        *position = v;
        mItemCount++;
    }

    iterator erase(const_iterator position) {
        std::move(position + 1, end(), position);
        mItemCount--;
    }

    iterator erase(const_iterator first, const_iterator last) {
        std::move(last, end(), first);
        mItemCount -= last - first;
    }

    void clear() noexcept {
        mItemCount = 0;
    }

    void resize(size_type count) {
        assert_capacity(count, sizeof(T));
        mItemCount = count;
    }

    void resize(size_type count, const_reference v) {
        assert_capacity(count, sizeof(T));
        if (count > size()) {
            std::generate(begin() + size(), begin() + count, [v] { return v; });
        }
        mItemCount = count;
    }

    void swap(vector& other) {
        TrivialVectorBase::swap(other);
    }

    void reserve(size_type n) {
        set_capacity(n, sizeof(T));
    }

    void shrink_to_fit() noexcept {
        set_capacity(size(), sizeof(T));
    }
};


} // end utils namespace

#endif //UTILS_VECTOR_H
