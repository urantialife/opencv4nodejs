#include "core.h"
#include "coreBindings.h"

NAN_MODULE_INIT(Core::Init) {
  Mat::Init(target);
  Point::Init(target);
  Vec::Init(target);
  Size::Init(target);
  Rect::Init(target);
  RotatedRect::Init(target);
  TermCriteria::Init(target);

  Nan::SetMethod(target, "getBuildInformation", GetBuildInformation);
  Nan::SetMethod(target, "partition", Partition);
  Nan::SetMethod(target, "kmeans", Kmeans);
  Nan::SetMethod(target, "cartToPolar", CartToPolar);
  Nan::SetMethod(target, "cartToPolarAsync", CartToPolarAsync);
  Nan::SetMethod(target, "polarToCart", PolarToCart);
  Nan::SetMethod(target, "polarToCartAsync", PolarToCartAsync);
  Nan::SetMethod(target, "getNumThreads", GetNumThreads);
  Nan::SetMethod(target, "setNumThreads", SetNumThreads);
  Nan::SetMethod(target, "getThreadNum", GetThreadNum);
};

NAN_METHOD(Core::GetBuildInformation) {
  info.GetReturnValue().Set(FF::newString(cv::getBuildInformation()));
}

template<class TClass, class TNativeObject>
static std::function<bool(TNativeObject, TNativeObject)> predicateFactory(v8::Local<v8::Function> cb) {
	 return [cb](TNativeObject pt1, TNativeObject pt2) {
		v8::Local<v8::Value> cbArgs[2];
		cbArgs[0] = TClass::Converter::wrap(pt1);
		cbArgs[1] = TClass::Converter::wrap(pt2);
		Nan::AsyncResource resource("opencv4nodejs:Predicate::Constructor");
		return FF::BoolConverter::unwrapUnchecked(resource.runInAsyncScope(Nan::GetCurrentContext()->Global(), cb, 2, cbArgs).ToLocalChecked());
	};
}

NAN_METHOD(Core::Partition) {
	FF::TryCatch tryCatch("Core::Partition");
  if (!info[0]->IsArray()) {
	  return tryCatch.throwError("expected arg 0 to be an array");
  }
  v8::Local<v8::Array> jsData = v8::Local<v8::Array>::Cast(info[0]);
  if (!info[1]->IsFunction()) {
	  return tryCatch.throwError("expected arg 1 to be a function");
  }
  if (jsData->Length() < 2) {
	  return tryCatch.throwError("expected data to contain atleast 2 elements");
  }

  v8::Local<v8::Function> cb = v8::Local<v8::Function>::Cast(info[1]);
  v8::Local<v8::Value> data0 = Nan::Get(jsData, 0).ToLocalChecked();

  int numLabels = 0;
  std::vector<int> labels;

  if (Point2::hasInstance(data0)) {
    std::vector<cv::Point2d> pts;
	if (Point2::ArrayConverter::arg(0, &pts, info)) {
		return tryCatch.reThrow();
	}

    numLabels = cv::partition(pts, labels, predicateFactory<Point2, cv::Point2d>(cb));
  }
  else if (Point3::hasInstance(data0)) {
    std::vector<cv::Point3d> pts;
	if (Point3::ArrayConverter::arg(0, &pts, info)) {
		return tryCatch.reThrow();
	}
	numLabels = cv::partition(pts, labels, predicateFactory<Point3, cv::Point3d>(cb));
  }
  else if (Vec2::hasInstance(data0)) {
	std::vector<cv::Vec2d> pts;
	if (Vec2::ArrayConverter::arg(0, &pts, info)) {
		return tryCatch.reThrow();
	}
    numLabels = cv::partition(pts, labels, predicateFactory<Vec2, cv::Vec2d>(cb));
  }
  else if (Vec3::hasInstance(data0)) {
	std::vector<cv::Vec3d> pts;
	if (Vec3::ArrayConverter::arg(0, &pts, info)) {
		return tryCatch.reThrow();
	}
	numLabels = cv::partition(pts, labels, predicateFactory<Vec3, cv::Vec3d>(cb));
  }
  else if (Vec4::hasInstance(data0)) {
	std::vector<cv::Vec4d> pts;
	if (Vec4::ArrayConverter::arg(0, &pts, info)) {
		return tryCatch.reThrow();
	}
    numLabels = cv::partition(pts, labels, predicateFactory<Vec4, cv::Vec4d>(cb));
  }
  else if (Mat::hasInstance(data0)) {
    std::vector<cv::Mat> mats;
	if (Mat::ArrayConverter::arg(0, &mats, info)) {
		return tryCatch.reThrow();
	}
    numLabels = cv::partition(mats, labels, predicateFactory<Mat, cv::Mat>(cb));
  }

  v8::Local<v8::Object> ret = Nan::New<v8::Object>();
  Nan::Set(ret, FF::newString("labels"), FF::IntArrayConverter::wrap(labels));
  Nan::Set(ret, FF::newString("numLabels"), Nan::New(numLabels));
  info.GetReturnValue().Set(ret);
}

NAN_METHOD(Core::Kmeans) {
	FF::TryCatch tryCatch("Core::Kmeans");
  if (!info[0]->IsArray()) {
	  return tryCatch.throwError("expected arg 0 to be an array");
  }
  v8::Local<v8::Array> jsData = v8::Local<v8::Array>::Cast(info[0]);
  
  if (jsData->Length() < 1) {
	  return tryCatch.throwError("expected data to contain at least 1 element");
  }
  
  v8::Local<v8::Value> data0 = Nan::Get(jsData, 0).ToLocalChecked();
  bool isPoint2 = Point2::hasInstance(data0);

  std::vector<cv::Point2f> pts2d;
  std::vector<cv::Point3f> pts3d;

  cv::TermCriteria termCriteria;
  int k, attempts, flags;
  if ((
	  (isPoint2 && Point2::ArrayWithCastConverter<cv::Point2f>::arg(0, &pts2d, info)) ||
	  (!isPoint2 && Point3::ArrayWithCastConverter<cv::Point3f>::arg(0, &pts3d, info))
	  ) ||
	  FF::IntConverter::arg(1, &k, info) ||
	  TermCriteria::Converter::arg(2, &termCriteria, info) ||
	  FF::IntConverter::arg(3, &attempts, info) ||
	  FF::IntConverter::arg(4, &flags, info)
	  ) {
	  return tryCatch.reThrow();
  }

  std::vector<int> labels;
  cv::Mat centersMat;
  
  if (isPoint2) {
    cv::kmeans(pts2d, k, labels, termCriteria, attempts, flags, centersMat);
  }
  else {
    cv::kmeans(pts3d, k, labels, termCriteria, attempts, flags, centersMat);
  }
  
  v8::Local<v8::Object> ret = Nan::New<v8::Object>();
  Nan::Set(ret, FF::newString("labels"), FF::IntArrayConverter::wrap(labels));

  if (Point2::hasInstance(data0)) {
    std::vector<cv::Point2f> centers;
    for (int i = 0; i < centersMat.rows; i++) {
      centers.push_back(cv::Point2f(centersMat.at<float>(i, 0), centersMat.at<float>(i, 1)));
    }
    Nan::Set(ret, FF::newString("centers"), Point2::ArrayWithCastConverter<cv::Point2f>::wrap(centers));
  }
  else if (Point3::hasInstance(data0)) {
    std::vector<cv::Point3f> centers;
    for (int i = 0; i < centersMat.rows; i++) {
      centers.push_back(cv::Point3f(centersMat.at<float>(i, 0), centersMat.at<float>(i, 1), centersMat.at<float>(i, 2)));
    }
	Nan::Set(ret, FF::newString("centers"), Point3::ArrayWithCastConverter<cv::Point3f>::wrap(centers));
  } 
  
  info.GetReturnValue().Set(ret);
}

NAN_METHOD(Core::CartToPolar) {
	FF::SyncBinding<CoreBindings::CartToPolar>("Core", "CartToPolar", info);
}

NAN_METHOD(Core::CartToPolarAsync) {
	FF::AsyncBinding<CoreBindings::CartToPolar>("Core", "CartToPolar", info);
}

NAN_METHOD(Core::PolarToCart) {
	FF::SyncBinding<CoreBindings::PolarToCart>("Core", "PolarToCart", info);
}

NAN_METHOD(Core::PolarToCartAsync) {
	FF::AsyncBinding<CoreBindings::PolarToCart>("Core", "PolarToCart", info);
}

NAN_METHOD(Core::GetNumThreads) {
  info.GetReturnValue().Set(FF::IntConverter::wrap(cv::getNumThreads()));
}

NAN_METHOD(Core::SetNumThreads) {
	FF::TryCatch tryCatch("Core::SetNumThreads");
  int num;
  if(FF::IntConverter::arg(0, &num, info)) {
    return tryCatch.reThrow();
  }
  cv::setNumThreads(num);
}

NAN_METHOD(Core::GetThreadNum) {
  info.GetReturnValue().Set(FF::IntConverter::wrap(cv::getThreadNum()));
}
