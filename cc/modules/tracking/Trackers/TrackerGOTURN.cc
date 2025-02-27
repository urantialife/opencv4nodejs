#ifdef HAVE_TRACKING

#include "TrackerGOTURN.h"

#if CV_MINOR_VERSION > 1

Nan::Persistent<v8::FunctionTemplate> TrackerGOTURN::constructor;

NAN_MODULE_INIT(TrackerGOTURN::Init) {
	v8::Local<v8::FunctionTemplate> ctor = Nan::New<v8::FunctionTemplate>(TrackerGOTURN::New);
	v8::Local<v8::ObjectTemplate> instanceTemplate = ctor->InstanceTemplate();

	Tracker::Init(ctor);

	constructor.Reset(ctor);
	ctor->SetClassName(FF::newString("TrackerGOTURN"));
	instanceTemplate->SetInternalFieldCount(1);

	Nan::Set(target,FF::newString("TrackerGOTURN"), FF::getFunction(ctor));
};


NAN_METHOD(TrackerGOTURN::New) {
	FF::TryCatch tryCatch("TrackerGOTURN::New");
	FF_ASSERT_CONSTRUCT_CALL();

	TrackerGOTURN* self = new TrackerGOTURN();
#if CV_VERSION_MINOR > 2
	self->tracker = cv::TrackerGOTURN::create();
#else
	self->tracker = cv::TrackerGOTURN::createTracker();
#endif

	self->Wrap(info.Holder());
	info.GetReturnValue().Set(info.Holder());
};

#endif

#endif
