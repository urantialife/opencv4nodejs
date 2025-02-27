#include "photo.h"
#include "photoBindings.h"

NAN_MODULE_INIT(Photo::Init) {
	Nan::SetMethod(target, "fastNlMeansDenoisingColored", FastNlMeansDenoisingColored);
	Nan::SetMethod(target, "fastNlMeansDenoisingColoredAsync", FastNlMeansDenoisingColoredAsync);
	Nan::SetMethod(target, "inpaint", Inpaint);
	Nan::SetMethod(target, "inpaintAsync", InpaintAsync);
};

NAN_METHOD(Photo::FastNlMeansDenoisingColored) {
	FF::SyncBindingBase(
		std::make_shared<PhotoBindings::FastNlMeansDenoisingColoredWorker>(),
		"Photo::FastNlMeansDenoisingColored",
		info
	);
}

NAN_METHOD(Photo::FastNlMeansDenoisingColoredAsync) {
	FF::AsyncBindingBase(
		std::make_shared<PhotoBindings::FastNlMeansDenoisingColoredWorker>(),
		"Photo::FastNlMeansDenoisingColoredAsync",
		info
	);
}

NAN_METHOD(Photo::Inpaint) {
  FF::SyncBindingBase(
    std::make_shared<PhotoBindings::InpaintWorker>(),
    "Photo::Inpaint",
    info
  );
}

NAN_METHOD(Photo::InpaintAsync) {
  FF::AsyncBindingBase(
    std::make_shared<PhotoBindings::InpaintWorker>(),
    "Photo::InpaintAsync",
    info
  );
}
