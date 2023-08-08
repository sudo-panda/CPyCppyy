#ifndef CPYCPPYY_CONVERTERS_H
#define CPYCPPYY_CONVERTERS_H

// Bindings
#include "Dimensions.h"

// Standard
#include <string>


namespace CPyCppyy {

struct Parameter;
struct CallContext;

class CPYCPPYY_CLASS_EXPORT Converter {
public:
    virtual ~Converter();

public:
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr) = 0;
    virtual PyObject* FromMemory(void* address);
    virtual bool ToMemory(PyObject* value, void* address, PyObject* ctxt = nullptr);
    virtual bool HasState() { return false; }
    virtual std::string GetFailureMsg() { return "[Converter]"; }
};

// create/destroy converter from fully qualified type (public API)
CPYCPPYY_EXPORT Converter* CreateConverter(const std::string& fullType, cdims_t dims = 0);
CPYCPPYY_EXPORT Converter* CreateConverter(Cppyy::TCppType_t type, cdims_t dims = 0);
CPYCPPYY_EXPORT void DestroyConverter(Converter* p);
typedef Converter* (*cf_t)(cdims_t d);
CPYCPPYY_EXPORT bool RegisterConverter(const std::string& name, cf_t fac);
CPYCPPYY_EXPORT bool UnregisterConverter(const std::string& name);


// converters for special cases (only here b/c of external use of StrictInstancePtrConverter)
class VoidArrayConverter : public Converter {
public:
    VoidArrayConverter(bool keepControl = true, const std::string &failureMsg = std::string()) 
        : fFailureMsg(failureMsg) { fKeepControl = keepControl; }

public:
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr);
    virtual PyObject* FromMemory(void* address);
    virtual bool ToMemory(PyObject* value, void* address, PyObject* ctxt = nullptr);
    virtual bool HasState() { return true; }
    virtual std::string GetFailureMsg() { return "[VoidArrayConverter] " + fFailureMsg; }

protected:
    virtual bool GetAddressSpecialCase(PyObject* pyobject, void*& address);
    bool KeepControl() { return fKeepControl; }
    const std::string fFailureMsg;

private:
    bool fKeepControl;
};

template <bool ISCONST>
class InstancePtrConverter : public VoidArrayConverter {
public:
    InstancePtrConverter(Cppyy::TCppType_t klass, bool keepControl = false, const std::string &failureMsg = std::string()) :
        VoidArrayConverter(keepControl, failureMsg), fClass(klass) {}

public:
    virtual bool SetArg(PyObject*, Parameter&, CallContext* = nullptr);
    virtual PyObject* FromMemory(void* address);
    virtual bool ToMemory(PyObject* value, void* address, PyObject* ctxt = nullptr);

protected:
    Cppyy::TCppType_t fClass;
};

class StrictInstancePtrConverter : public InstancePtrConverter<false> {
public:
    using InstancePtrConverter<false>::InstancePtrConverter;

protected:
    virtual bool GetAddressSpecialCase(PyObject*, void*&) { return false; }
};

} // namespace CPyCppyy

#endif // !CPYCPPYY_CONVERTERS_H
