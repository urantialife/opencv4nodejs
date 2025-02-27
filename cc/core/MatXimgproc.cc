#ifdef HAVE_XIMGPROC

#include "MatXimgproc.h"
#include "MatXimgprocBindings.h"

void MatXimgproc::Init(v8::Local<v8::FunctionTemplate> ctor) {
  Nan::SetPrototypeMethod(ctor, "guidedFilter", GuidedFilter);
  Nan::SetPrototypeMethod(ctor, "guidedFilterAsync", GuidedFilterAsync);
};

NAN_METHOD(MatXimgproc::GuidedFilter) {
  FF::SyncBindingBase(
    std::make_shared<MatXimgprocBindings::GuidedFilterWorker>(Mat::unwrapSelf(info)),
    "Mat::GuidedFilter",
    info
  );
}

NAN_METHOD(MatXimgproc::GuidedFilterAsync) {
  FF::AsyncBindingBase(
    std::make_shared<MatXimgprocBindings::GuidedFilterWorker>(Mat::unwrapSelf(info)),
    "Mat::GuidedFilterAsync",
    info
  );
}

#endif // HAVE_XIMGPROC
