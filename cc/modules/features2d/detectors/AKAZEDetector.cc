#include "AKAZEDetector.h"

Nan::Persistent<v8::FunctionTemplate> AKAZEDetector::constructor;

NAN_MODULE_INIT(AKAZEDetector::Init) {
  v8::Local<v8::FunctionTemplate> ctor = Nan::New<v8::FunctionTemplate>(AKAZEDetector::New);
	v8::Local<v8::ObjectTemplate> instanceTemplate = ctor->InstanceTemplate();

	FeatureDetector::Init(ctor);
  constructor.Reset(ctor);
	ctor->SetClassName(Nan::New("AKAZEDetector").ToLocalChecked());
  instanceTemplate->SetInternalFieldCount(1);

	Nan::SetAccessor(instanceTemplate, Nan::New("descriptorType").ToLocalChecked(), descriptorType_getter);
	Nan::SetAccessor(instanceTemplate, Nan::New("descriptorSize").ToLocalChecked(), descriptorSize_getter);
	Nan::SetAccessor(instanceTemplate, Nan::New("descriptorChannels").ToLocalChecked(), descriptorChannels_getter);
	Nan::SetAccessor(instanceTemplate, Nan::New("threshold").ToLocalChecked(), threshold_getter);
	Nan::SetAccessor(instanceTemplate, Nan::New("nOctaves").ToLocalChecked(), nOctaves_getter);
	Nan::SetAccessor(instanceTemplate, Nan::New("nOctaveLayers").ToLocalChecked(), nOctaveLayers_getter);
	Nan::SetAccessor(instanceTemplate, Nan::New("diffusivity").ToLocalChecked(), diffusivity_getter);

  Nan::Set(target,Nan::New("AKAZEDetector").ToLocalChecked(), FF::getFunction(ctor));
};

NAN_METHOD(AKAZEDetector::New) {
	FF::TryCatch tryCatch("AKAZEDetector::New");
	FF_ASSERT_CONSTRUCT_CALL();
	AKAZEDetector::NewWorker worker;

	if (worker.applyUnwrappers(info)) {
		return tryCatch.reThrow();
	}

	AKAZEDetector* self = new AKAZEDetector();
	self->self = cv::AKAZE::create(worker.descriptorType, worker.descriptorSize, worker.descriptorChannels, worker.threshold, worker.nOctaves, worker.nOctaveLayers, worker.diffusivity);
	self->Wrap(info.Holder());
	info.GetReturnValue().Set(info.Holder());
}