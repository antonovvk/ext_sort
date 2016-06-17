#pragma once

#include <algorithm>
#include <memory>
#include <fstream>
#include <vector>
#include <queue>

#include "traits.h"

namespace ext_sort {
    template <typename T>
    class VectorIter {
    public:
        typedef T Value;

    public:
        VectorIter(const std::vector<T>& data)
            : Data_(data)
            , Curr_(-1)
        {
        }

        bool Next() {
            ++Curr_;
            return Curr_ != (int)Data_.size();
        }

        const T& Current() const {
            return Data_[Curr_];
        }

    private:
        const std::vector<T>& Data_;
        int Curr_;
    };

    template <typename T, typename Traits, typename Context>
    class HeapIter {
        class Input {
        public:
            Input(const std::string& fileName, Context* ctx)
                : Name_(fileName)
                , File_(Name_, std::ios_base::in | std::ios_base::binary)
                , Context_(ctx)
            {
            }

            ~Input()
            {
            }

            bool Next() {
                return Traits::Load(File_, Value_, Context_);
            }

            const T& Current() const {
                return Value_;
            }

            bool operator < (const Input& in) const {
                return Traits::Compare(Current(), in.Current(), Context_) < 0;
            }

        private:
            std::string Name_;
            std::ifstream File_;
            Context* Context_;
            T Value_;
        };

        struct PtrCmp {
            bool operator() (const Input* a, const Input* b) {
                return *b < *a;
            }
        };

    public:
        typedef T Value;

    public:
        HeapIter(const std::vector<std::string>& fileNames, Context* ctx)
            : Started_(false)
        {
            for (auto& fileName: fileNames) {
                Inputs_.emplace_back(new Input(fileName, ctx));
                if (Inputs_.back()->Next()) {
                    Queue_.push(Inputs_.back().get());
                }
            }
        }

        HeapIter(const HeapIter& h) = delete;

        bool Next() {
            if (Queue_.empty()) {
                return false;
            }
            if (!Started_) {
                Started_ = true;
                return true;
            }
            auto curr = Queue_.top();
            Queue_.pop();
            if (curr->Next()) {
                Queue_.push(curr);
            }
            return !Queue_.empty();
        }

        const T& Current() const {
            return Queue_.top()->Current();
        }

    private:
        std::vector<std::unique_ptr<Input>> Inputs_;
        std::priority_queue<Input*, std::vector<Input*>, PtrCmp> Queue_;
        bool Started_;
    };

    template <typename T, typename Traits, typename Context>
    class ResultIter {
    public:
        ResultIter()
            : Curr_(nullptr)
        {
        }

        ResultIter(const std::vector<T>& data)
            : VecIt_(new VectorIter<T>(data))
            , Curr_(nullptr)
        {
            Advance();
        }

        ResultIter(const std::vector<std::string>& tmpFileNames, Context* ctx)
            : HeapIt_(new HeapIter<T, Traits, Context>(tmpFileNames, ctx))
            , Curr_(nullptr)
        {
            Advance();
        }

        ResultIter& operator ++ () {
            Advance();
            return *this;
        }

        bool operator != (const ResultIter& it) const {
            return VecIt_.get() != it.VecIt_.get() || HeapIt_.get() != it.HeapIt_.get();
        }

        const T& operator* () const {
            assert(Curr_);
            return *Curr_;
        }

    private:
        void Advance() {
            if (VecIt_) {
                DoAdvance(VecIt_);
            } else if (HeapIt_) {
                DoAdvance(HeapIt_);
            }
        }

        template <typename Iter>
        void DoAdvance(Iter& it) {
            if (it->Next()) {
                Curr_ = &it->Current();
            } else {
                Curr_ = nullptr;
                it = nullptr;
            }
        }

    private:
        std::shared_ptr<VectorIter<T>> VecIt_;
        std::shared_ptr<HeapIter<T, Traits, Context>> HeapIt_;
        const T* Curr_;
    };
} // namespace ext_sort.h
