#ifdef HAVE_TEXT

#include "OCRHMMDecoder.h"
#include "OCRHMMDecoderBindings.h"

Nan::Persistent<v8::FunctionTemplate> OCRHMMDecoder::constructor;

NAN_MODULE_INIT(OCRHMMDecoder::Init) {
  v8::Local<v8::FunctionTemplate> ctor = Nan::New<v8::FunctionTemplate>(OCRHMMDecoder::New);
  v8::Local<v8::ObjectTemplate> instanceTemplate = ctor->InstanceTemplate();

  constructor.Reset(ctor);
  ctor->SetClassName(FF::newString("OCRHMMDecoder"));
  instanceTemplate->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(ctor, "run", Run);
  Nan::SetPrototypeMethod(ctor, "runAsync", RunAsync);
  Nan::SetPrototypeMethod(ctor, "runWithInfo", RunWithInfo);
  Nan::SetPrototypeMethod(ctor, "runWithInfoAsync", RunWithInfoAsync);

  Nan::Set(target,FF::newString("OCRHMMDecoder"), FF::getFunction(ctor));
};

NAN_METHOD(OCRHMMDecoder::New) {
	FF::TryCatch tryCatch("OCRHMMDecoder::New");
	FF_ASSERT_CONSTRUCT_CALL();
	OCRHMMDecoderBindings::NewWorker worker;

	if (worker.applyUnwrappers(info)) {
		return tryCatch.reThrow();
	}

	OCRHMMDecoder* self = new OCRHMMDecoder();
	self->setNativeObject(cv::text::OCRHMMDecoder::create(
		worker.classifier,
		worker.vocabulary,
		worker.transition_probabilities_table,
		worker.emission_probabilities_table
#if CV_MINOR_VERSION > 0
		, worker.mode
#endif
	));

	self->Wrap(info.Holder());
	info.GetReturnValue().Set(info.Holder());
}

NAN_METHOD(OCRHMMDecoder::Run) {
  FF::SyncBindingBase(
	std::make_shared<OCRHMMDecoderBindings::RunWorker>(OCRHMMDecoder::unwrapSelf(info)),
    "OCRHMMDecoder::Run",
    info
  );
}

NAN_METHOD(OCRHMMDecoder::RunAsync) {
  FF::AsyncBindingBase(
    std::make_shared<OCRHMMDecoderBindings::RunWorker>(OCRHMMDecoder::unwrapSelf(info)),
    "OCRHMMDecoder::RunAsync",
    info
  );
}

NAN_METHOD(OCRHMMDecoder::RunWithInfo) {
  FF::SyncBindingBase(
    std::make_shared<OCRHMMDecoderBindings::RunWithInfoWorker>(OCRHMMDecoder::unwrapSelf(info)),
    "OCRHMMDecoder::RunWithInfo",
    info
  );
}

NAN_METHOD(OCRHMMDecoder::RunWithInfoAsync) {
  FF::AsyncBindingBase(
    std::make_shared<OCRHMMDecoderBindings::RunWithInfoWorker>(OCRHMMDecoder::unwrapSelf(info)),
    "OCRHMMDecoder::RunWithInfoAsync",
    info
  );
}

#endif
