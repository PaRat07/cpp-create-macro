#pragma once

#include <algorithm>

#include <boost/preprocessor.hpp>


template<typename WrappedT>
struct Wrapper {
  using unwrp = WrappedT;
};

template <typename T>
struct Getter {
  friend constexpr auto Magic(Getter<T>);
};

template <typename T, typename Value>
struct Injector {
  friend constexpr auto Magic(Getter<T>) {return Value{};};
};

template<typename What, auto Uniquefy = [] {}>
inline constexpr bool kDoesExist = requires { (Magic(Getter<What>{}), Uniquefy());};

template<typename...>
struct TypeList {};

template<auto>
struct ValueWrapper {};

template<int Sz>
struct ConstexprString {
  consteval ConstexprString() = default;

  consteval ConstexprString(const char (&val)[Sz]) {
    std::ranges::copy(val, data);
  }

  char data[Sz]{};
};

template<int kSz>
constexpr int GetPrefixIdLen(ConstexprString<kSz> q) {
  int ans = 0;
  while (ans < kSz && (q.data[ans] >= 'a' && q.data[ans] <= 'z' ||
                       q.data[ans] == '_' ||
                       q.data[ans] >= '0' && q.data[ans] <= '9')) {
    ++ans;
  }
  return ans;
}

template<ConstexprString str>
static constexpr auto kIdOfDecl = [] consteval {
  static constexpr int kAnsLen = GetPrefixIdLen(str);
  ConstexprString<kAnsLen> ans;
  for (int i = 0; i < kAnsLen; ++i) {
    ans.data[i] = str.data[i];
  }
  return ans;
} ();

struct CachedTypeForCreateTag {};

#define GET_TYPE(DECLAR) decltype([] { auto DECLAR; using T = std::decay_t<decltype(DECLAR)>; return Wrapper<T>(); } ())::unwrp
#define DECL_VAR(r, data, VAR) GET_TYPE(VAR) VAR;
#define DECLARE(...) BOOST_PP_SEQ_FOR_EACH(DECL_VAR, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
#define GET_TYPE_HASH_DECL_VAR(...) decltype(TypeList<CachedTypeForCreateTag, ValueWrapper<kIdOfDecl<#__VA_ARGS__>>, GET_TYPE(__VA_ARGS__)>{})
#define APPLY_GET_TYPE_HASH_DECL_VAR(s, data, x) GET_TYPE_HASH_DECL_VAR(x)

// Transforms each argument in the tuple and expands them as a comma-separated list
#define GET_TYPE_HASH_DECL_VAR_EXPANDED(...) \
  BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(APPLY_GET_TYPE_HASH_DECL_VAR, _, BOOST_PP_TUPLE_TO_SEQ((__VA_ARGS__))))


#define GET_TYPE_HASH_DECL_STRUCT(...) \
  TypeList<GET_TYPE_HASH_DECL_VAR_EXPANDED(__VA_ARGS__)>
#define MAKE_DOTTED(s, data, x) .x


#define CREATE(...) decltype([] { \
  using HashT = GET_TYPE_HASH_DECL_STRUCT(__VA_ARGS__); \
  using T = struct { DECLARE(__VA_ARGS__) }; \
  std::ignore = Injector<std::conditional_t<kDoesExist<HashT>, decltype([]{}), HashT>, Wrapper<T>>{};\
  return Magic(Getter<HashT>{});\
} ())::unwrp{BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(MAKE_DOTTED, _, BOOST_PP_TUPLE_TO_SEQ((__VA_ARGS__))))}
