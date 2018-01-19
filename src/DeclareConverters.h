#ifndef CPYCPPYY_DECLARECONVERTERS_H
#define CPYCPPYY_DECLARECONVERTERS_H

// Bindings
#include "Converters.h"

// Standard
#include <string>


namespace CPyCppyy {

namespace {

#define CPPYY_DECLARE_BASIC_CONVERTER(name)                                  \
class name##Converter : public Converter {                                   \
public:                                                                      \
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr);      \
    virtual PyObject* FromMemory(void*);                                     \
    virtual bool ToMemory(PyObject*, void*);                                 \
};                                                                           \
                                                                             \
class Const##name##RefConverter : public Converter {                         \
public:                                                                      \
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr);      \
}


#define CPPYY_DECLARE_BASIC_CONVERTER2(name, base)                           \
class name##Converter : public base##Converter {                             \
public:                                                                      \
    virtual PyObject* FromMemory(void*);                                     \
    virtual bool ToMemory(PyObject*, void*);                                 \
};                                                                           \
                                                                             \
class Const##name##RefConverter : public Converter {                         \
public:                                                                      \
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr);      \
}

#define CPPYY_DECLARE_REF_CONVERTER(name)                                    \
class name##RefConverter : public Converter {                                \
public:                                                                      \
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr);      \
};

#define CPPYY_DECLARE_ARRAY_CONVERTER(name)                                  \
class name##Converter : public Converter {                                   \
public:                                                                      \
    name##Converter(Py_ssize_t size = -1) { fSize = size; }                  \
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr);      \
    virtual PyObject* FromMemory(void*);                                     \
    virtual bool ToMemory(PyObject*, void*);                                 \
private:                                                                     \
    Py_ssize_t fSize;                                                        \
};                                                                           \
                                                                             \
class name##RefConverter : public name##Converter {                          \
public:                                                                      \
    using name##Converter::name##Converter;                                  \
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr);      \
}

// converters for built-ins
CPPYY_DECLARE_BASIC_CONVERTER(Long);
CPPYY_DECLARE_BASIC_CONVERTER(Bool);
CPPYY_DECLARE_BASIC_CONVERTER(Char);
CPPYY_DECLARE_BASIC_CONVERTER(UChar);
CPPYY_DECLARE_BASIC_CONVERTER(Short);
CPPYY_DECLARE_BASIC_CONVERTER(UShort);
CPPYY_DECLARE_BASIC_CONVERTER(Int);
CPPYY_DECLARE_BASIC_CONVERTER(ULong);
CPPYY_DECLARE_BASIC_CONVERTER2(UInt, ULong);
CPPYY_DECLARE_BASIC_CONVERTER(LongLong);
CPPYY_DECLARE_BASIC_CONVERTER(ULongLong);
CPPYY_DECLARE_BASIC_CONVERTER(Double);
CPPYY_DECLARE_BASIC_CONVERTER(Float);
CPPYY_DECLARE_BASIC_CONVERTER(LongDouble);

CPPYY_DECLARE_REF_CONVERTER(Int);
CPPYY_DECLARE_REF_CONVERTER(Long);
CPPYY_DECLARE_REF_CONVERTER(Double);

class VoidConverter : public Converter {
public:
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr);
};

class CStringConverter : public Converter {
public:
    CStringConverter(long maxSize = -1) : fMaxSize(maxSize) {}

public:
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr);
    virtual PyObject* FromMemory(void* address);
    virtual bool ToMemory(PyObject* value, void* address);

protected:
    std::string fBuffer;
    long fMaxSize;
};

class NonConstCStringConverter : public CStringConverter {
public:
    NonConstCStringConverter(long maxSize = -1) : CStringConverter(maxSize) {}

public:
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr);
    virtual PyObject* FromMemory(void* address);
};

// pointer/array conversions
CPPYY_DECLARE_ARRAY_CONVERTER(BoolArray);
CPPYY_DECLARE_ARRAY_CONVERTER(UCharArray);
CPPYY_DECLARE_ARRAY_CONVERTER(ShortArray);
CPPYY_DECLARE_ARRAY_CONVERTER(UShortArray);
CPPYY_DECLARE_ARRAY_CONVERTER(IntArray);
CPPYY_DECLARE_ARRAY_CONVERTER(UIntArray);
CPPYY_DECLARE_ARRAY_CONVERTER(LongArray);
CPPYY_DECLARE_ARRAY_CONVERTER(ULongArray);
CPPYY_DECLARE_ARRAY_CONVERTER(LLongArray);
CPPYY_DECLARE_ARRAY_CONVERTER(ULLongArray);
CPPYY_DECLARE_ARRAY_CONVERTER(FloatArray);
CPPYY_DECLARE_ARRAY_CONVERTER(DoubleArray);

class LongLongArrayConverter : public VoidArrayConverter {
public:
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr);
};

// converters for special cases
class ValueCppObjectConverter : public StrictCppObjectConverter {
public:
    using StrictCppObjectConverter::StrictCppObjectConverter;
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr);
};

class RefCppObjectConverter : public Converter  {
public:
    RefCppObjectConverter(Cppyy::TCppType_t klass) : fClass(klass) {}

public:
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr);

protected:
    Cppyy::TCppType_t fClass;
};

class MoveCppObjectConverter : public RefCppObjectConverter  {
public:
    using RefCppObjectConverter::RefCppObjectConverter;
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr);
};

template <bool ISREFERENCE>
class CppObjectPtrConverter : public CppObjectConverter {
public:
    using CppObjectConverter::CppObjectConverter;

public:
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr);
    virtual PyObject* FromMemory(void* address);
    virtual bool ToMemory(PyObject* value, void* address);
};

extern template class CppObjectPtrConverter<true>;
extern template class CppObjectPtrConverter<false>;

class CppObjectArrayConverter : public CppObjectConverter {
public:
    CppObjectArrayConverter(Cppyy::TCppType_t klass, size_t size, bool keepControl = false) :
        CppObjectConverter(klass, keepControl), m_size(size) {}

public:
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr);
    virtual PyObject* FromMemory(void* address);
    virtual bool ToMemory(PyObject* value, void* address);

protected:
    size_t m_size;
};

// CLING WORKAROUND -- classes for STL iterators are completely undefined in that
// they come in a bazillion different guises, so just do whatever
class STLIteratorConverter : public Converter {
public:
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr);
};
// -- END CLING WORKAROUND

class VoidPtrRefConverter : public Converter {
public:
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr);
};

class VoidPtrPtrConverter : public Converter {
public:
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr);
    virtual PyObject* FromMemory(void* address);
};

CPPYY_DECLARE_BASIC_CONVERTER(PyObject);

#define CPPYY_DECLARE_STRING_CONVERTER(name, strtype)                        \
class name##Converter : public CppObjectConverter {                          \
public:                                                                      \
    name##Converter(bool keepControl = true);                                \
public:                                                                      \
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr);      \
    virtual PyObject* FromMemory(void* address);                             \
    virtual bool ToMemory(PyObject* value, void* address);                   \
private:                                                                     \
    strtype fBuffer;                                                         \
}

CPPYY_DECLARE_STRING_CONVERTER(STLString, std::string);
#if __cplusplus > 201402L
CPPYY_DECLARE_STRING_CONVERTER(STLStringView, std::string_view);
#endif

// smart pointer converter
class SmartPtrCppObjectConverter : public Converter {
public:
    SmartPtrCppObjectConverter(Cppyy::TCppType_t smart,
                               Cppyy::TCppType_t raw,
                               Cppyy::TCppMethod_t deref,
                               bool keepControl = false,
                               bool isRef = false)
        : fSmartPtrType(smart), fRawPtrType(raw), fDereferencer(deref),
          fKeepControl(keepControl), fIsRef(isRef) {}

public:
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr);
    virtual PyObject* FromMemory(void* address);
    //virtual bool ToMemory(PyObject* value, void* address);

protected:
    virtual bool GetAddressSpecialCase(PyObject*, void*&) { return false; }

    Cppyy::TCppType_t   fSmartPtrType;
    Cppyy::TCppType_t   fRawPtrType;
    Cppyy::TCppMethod_t fDereferencer;
    bool                fKeepControl;
    bool                fIsRef;
};

// initializer lists
class InitializerListConverter : public Converter {
public:
    InitializerListConverter(Converter* cnv, size_t sz) :
        fConverter(cnv), fValueSize(sz) {}
    ~InitializerListConverter() {
        delete fConverter;
    }

public:
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr);

protected:
    Converter* fConverter;
    size_t     fValueSize;
};

// raising converter to take out overloads
class NotImplementedConverter : public Converter {
public:
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr);
};

} // unnamed namespace

} // namespace CPyCppyy

#endif // !CPYCPPYY_DECLARECONVERTERS_H
