#ifndef THORSANVIL_DB_MONGO_MONGO_CURSOR_H
#define THORSANVIL_DB_MONGO_MONGO_CURSOR_H

#include "ThorsMongoConfig.h"
#include "ThorsMongoCommon.h"
#include "ThorSerialize/Traits.h"

namespace ThorsAnvil::DB::Mongo
{

class Cursor
{
    friend class ThorsAnvil::Serialize::Traits<Cursor>;
    bool                        partialResultsReturned  = false;
    std::uint64_t               id                      = 0;
    std::string                 ns;
    public:
        std::uint64_t           getId() const {return id;}
        friend void swap(Cursor& lhs, Cursor& rhs) noexcept
        {
            using std::swap;
            swap(lhs.partialResultsReturned, rhs.partialResultsReturned);
            swap(lhs.id,                     rhs.id);
            swap(lhs.ns,                     rhs.ns);
        }
};

template<typename T>
class CursorFirst;
template<typename T>
class CursorNext: public Cursor
{
    friend class ThorsAnvil::Serialize::Traits<CursorNext<T>>;
    public:
        std::vector<T>          nextBatch;
        std::vector<T> const&   data()  const {return nextBatch;}
};

template<typename T>
class FindResult;
template<typename T>
class CursorFirst: public Cursor
{
    friend class ThorsAnvil::Serialize::Traits<CursorFirst<T>>;
    friend class FindResult<T>;

    std::vector<T>              firstBatch;
    public:
        std::vector<T> const&   data()  const {return firstBatch;}
        friend void swap(CursorFirst<T>& lhs, CursorNext<T>& rhs) noexcept
        {
            using std::swap;
            swap(static_cast<Cursor&>(lhs), static_cast<Cursor&>(rhs));
            swap(lhs.firstBatch,            rhs.nextBatch);
        }
};

template<typename T>
class CursorIterator
{
    Nref<FindResult<T>>     findResult;
    bool                    input;

    public:
        using difference_type   = std::ptrdiff_t;
        using value_type        = std::remove_cv_t<T>;
        using pointer           = T*;
        using reference         = T&;
        using iterator_category = std::input_iterator_tag;
        CursorIterator(FindResult<T>& findResult, bool input)
            : findResult(findResult)
            , input(input)
        {}

        // Input iterator.
        // If they are both the end they are equal.
        // Since this is an input iterator then if they are not the end they are also always equal.
        friend bool operator==(CursorIterator const& lhs, CursorIterator const& rhs)    {return lhs.input == rhs.input;}
        friend bool operator!=(CursorIterator const& lhs, CursorIterator const& rhs)    {return !(lhs == rhs);}

        T& operator*()  {return findResult.get().current();}
        T* operator->() {return &findResult.get().current();}
        CursorIterator& operator++()     /* prefix */
        {
            // increment() returns true if more data is
            // available after getting more data.
            input = findResult.get().increment();
            return *this;
        }
        // Postfix ++ is not supported on input iterators.
        //CursorIterator  operator++(int)  /* postfix */
};

template<typename T, typename I = CursorIterator<T>>
struct Range
{
    std::shared_ptr<FindResult<T>>   findData;
    public:
        Range(std::unique_ptr<FindResult<T>> result)
            : findData(std::move(result))
        {}
        CursorIterator<T> begin()   {return CursorIterator<T>{*findData, true};}
        CursorIterator<T> end()     {return CursorIterator<T>{*findData, false};}

    private:
        friend class TestFindResult<T>;
        FindResult<T>& getResult() const {return *findData;}
};

}

ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::Cursor,                  partialResultsReturned, id, ns);
ThorsAnvil_Template_ExpandTrait(1,
                        ThorsAnvil::DB::Mongo::Cursor,
                        ThorsAnvil::DB::Mongo::CursorFirst,             firstBatch);
ThorsAnvil_Template_ExpandTrait(1,
                        ThorsAnvil::DB::Mongo::Cursor,
                        ThorsAnvil::DB::Mongo::CursorNext,              nextBatch);

#endif
