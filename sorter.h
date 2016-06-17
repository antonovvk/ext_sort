#pragma once

#include <sys/types.h>
#include <unistd.h>

#include <fstream>
#include <sstream>
#include <string>

#include "traits.h"
#include "iterators.h"

namespace ext_sort {
    template <typename T, typename Traits = ObjectTraits<T>, typename Context = typename Traits::Context>
    class Sorter {
    public:
        typedef ResultIter<T, Traits, Context> Iterator;

    public:
        Sorter(size_t bufSize, const std::string& tmpPref, Context* ctx = nullptr)
            : BufSize_(bufSize)
            , TmpPref_(tmpPref)
            , Context_(ctx)
            , CurrSize_(0)
            , Sorted_(false)
        {
        }

        ~Sorter()
        {
            Clear();
        }

        void Add(const T& val) {
            Data_.push_back(val);
            Add();
        }

        void Add(T&& val) {
            Data_.emplace_back(std::move(val));
            Add();
        }

        Iterator begin() {
            Finish();
            if (TmpFileNames_.empty()) {
                return Iterator(Data_);
            }
            return Iterator(TmpFileNames_, Context_);
        }

        Iterator end() const {
            return Iterator();
        }

        template <typename Handler>
        void Iterate(Handler& handle) {
            Finish();
            if (TmpFileNames_.empty()) {
                VectorIter<T> result(Data_);
                handle(result);
            } else {
                HeapIter<T, Traits, Context> result(TmpFileNames_, Context_);
                handle(result);
            }
        }

        void Clear() {
            std::vector<T>().swap(Data_);
            CurrSize_ = 0;

            for (auto fileName: TmpFileNames_) {
                remove(fileName.c_str());
            }
            TmpFileNames_.clear();
        }

    private:
        void Add() {
            CurrSize_ += Traits::Size(Data_.back());
            if (CurrSize_ >= BufSize_) {
                WritePortion();
                Data_.clear();
                CurrSize_ = 0;
            }
            Sorted_ = false;
        }

        void Sort() {
            auto ctx = Context_;
            std::sort(
                Data_.begin(),
                Data_.end(),
                [ ctx ] (const T& a, const T& b) {
                    return Traits::Compare(a, b, ctx) < 0;
                }
            );
        }

        void WritePortion() {
            Sort();

            std::ostringstream fileName;
            fileName << TmpPref_ << "." << ::getpid() << "." << TmpFileNames_.size();
            TmpFileNames_.push_back(fileName.str());

            std::ofstream tmp(TmpFileNames_.back(), std::ios_base::out | std::ios_base::binary);
            for (auto& val: Data_) {
                Traits::Save(tmp, val, Context_);
            }
        }

        void Finish() {
            if (Sorted_) {
                return;
            }
            if (TmpFileNames_.empty()) {
                Sort();
            } else {
                WritePortion();
            }
            Sorted_ = true;
        }

    private:
        const size_t BufSize_;
        const std::string TmpPref_;
        Context* Context_;
        std::vector<std::string> TmpFileNames_;
        std::vector<T> Data_;
        size_t CurrSize_;
        bool Sorted_;
    };
} // namespace ext_sort
