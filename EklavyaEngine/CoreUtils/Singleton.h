#ifndef _INC_UTILS_SINGLETON_H_
#define _INC_UTILS_SINGLETON_H_

template <typename Type> class Singleton {
public:
  static Type &GetInstance() {
    static Type instance = Type();
    return instance;
  }

protected:
  Singleton() {}
};

#endif
