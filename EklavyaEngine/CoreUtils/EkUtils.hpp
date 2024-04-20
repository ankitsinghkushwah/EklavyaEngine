//
//  EkUtils.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 20/05/2023.
//

#ifndef EkUtils_hpp
#define EkUtils_hpp

#include <type_traits>
#include <typeinfo>
#include <iterator>
#include <vector>
#include <functional>

/*to create unique ids, we just take location of type_id<T>, this idea is
taken from Feral programming language implementation*/

template <typename Type> const inline std::uintptr_t get_uid() {
  return reinterpret_cast<std::uintptr_t>(&typeid(Type));
}

#endif /* EkUtils_hpp */
