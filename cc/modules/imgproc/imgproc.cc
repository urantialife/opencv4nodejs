#include "imgproc.h"
#include "imgprocBindings.h"

#define FF_DEFINE_CALC_HIST(name, n, constRangesArray)                                                        \
  cv::MatND name(cv::Mat img, cv::Mat mask, int channels[], int histSize[], std::vector<float*> rangesVec) {  \
    const float* ranges[] = constRangesArray;                                                                  \
    cv::MatND hist;                                                                                            \
    cv::calcHist(&img, 1, channels, mask, hist, n, histSize, ranges, true, false);                            \
    return hist;                                                                                              \
  }

#define FF_HIST_RANGE_1 { rangesVec.at(0) }
#define FF_HIST_RANGE_2 { rangesVec.at(0), rangesVec.at(1) }
#define FF_HIST_RANGE_3 { rangesVec.at(0), rangesVec.at(1), rangesVec.at(2) }
#define FF_HIST_RANGE_4 { rangesVec.at(0), rangesVec.at(1), rangesVec.at(2), rangesVec.at(3) }

FF_DEFINE_CALC_HIST(calcHist1, 1, FF_HIST_RANGE_1);
FF_DEFINE_CALC_HIST(calcHist2, 2, FF_HIST_RANGE_2);
FF_DEFINE_CALC_HIST(calcHist3, 3, FF_HIST_RANGE_3);
FF_DEFINE_CALC_HIST(calcHist4, 4, FF_HIST_RANGE_4);

NAN_MODULE_INIT(Imgproc::Init) {
  Nan::SetMethod(target, "getStructuringElement", GetStructuringElement);
  Nan::SetMethod(target, "getRotationMatrix2D", GetRotationMatrix2D);
  Nan::SetMethod(target, "calcHist", CalcHist);
  Nan::SetMethod(target, "plot1DHist", Plot1DHist);
  Nan::SetMethod(target, "fitLine", FitLine);
  Nan::SetMethod(target, "getAffineTransform", GetAffineTransform);
  Nan::SetMethod(target, "getPerspectiveTransform", GetPerspectiveTransform);
  Nan::SetMethod(target, "undistortPoints", UndistortPoints);
  Nan::SetMethod(target, "undistortPointsAsync", UndistortPointsAsync);
  Nan::SetMethod(target, "getTextSize", GetTextSize);
  Nan::SetMethod(target, "getTextSizeAsync", GetTextSizeAsync);
  Nan::SetMethod(target, "applyColorMap", ApplyColorMap);
  Nan::SetMethod(target, "applyColorMapAsync", ApplyColorMapAsync);
#if CV_VERSION_MINOR > 1
  Nan::SetMethod(target, "canny", Canny);
  Nan::SetMethod(target, "cannyAsync", CannyAsync);
#endif
  Moments::Init(target);
  Contour::Init(target);
};

NAN_METHOD(Imgproc::GetStructuringElement) {
	FF::TryCatch tryCatch("Imgproc::GetStructuringElement");

	uint shape;
	cv::Size2d size;
	cv::Point2d anchor = cv::Point2d(-1, -1);

	if (
		FF::UintConverter::arg(0, &shape, info) ||
		Size::Converter::arg(1, &size, info) ||
		Point2::Converter::optArg(2, &anchor, info)
	) {
		return tryCatch.reThrow();
	}

	info.GetReturnValue().Set(Mat::Converter::wrap(cv::getStructuringElement(shape, size, anchor)));
}

NAN_METHOD(Imgproc::GetRotationMatrix2D) {
	FF::TryCatch tryCatch("Imgproc::GetRotationMatrix2D");

	cv::Point2d center;
	double angle, scale = 1.0;
	if (
		Point2::Converter::arg(0, &center, info) ||
		FF::DoubleConverter::arg(1, &angle, info) ||
		FF::DoubleConverter::optArg(2, &scale, info)
		) {
		return tryCatch.reThrow();
	}

	info.GetReturnValue().Set(Mat::Converter::wrap(cv::getRotationMatrix2D(center, angle, scale)));
}

NAN_METHOD(Imgproc::GetAffineTransform) {
	FF::TryCatch tryCatch("Imgproc::GetAffineTransform");

	std::vector<cv::Point2f> srcPoints, dstPoints;
	if (
		Point2::ArrayWithCastConverter<cv::Point2f>::arg(0, &srcPoints, info) ||
		Point2::ArrayWithCastConverter<cv::Point2f>::arg(1, &dstPoints, info)
		) {
		return tryCatch.reThrow();
	}

	info.GetReturnValue().Set(Mat::Converter::wrap(cv::getAffineTransform(srcPoints, dstPoints)));
}

NAN_METHOD(Imgproc::GetPerspectiveTransform) {
  FF::TryCatch tryCatch("Imgproc::GetPerspectiveTransform");

  std::vector<cv::Point2f> srcPoints, dstPoints;
  if (Point2::ArrayWithCastConverter<cv::Point2f>::arg(0, &srcPoints, info)
	  || Point2::ArrayWithCastConverter<cv::Point2f>::arg(1, &dstPoints, info)
	) {
	  return tryCatch.reThrow();
  }

  info.GetReturnValue().Set(Mat::Converter::wrap(cv::getPerspectiveTransform(srcPoints, dstPoints)));
}
NAN_METHOD(Imgproc::UndistortPoints) {
  FF::SyncBindingBase(std::make_shared<ImgprocBindings::UndistortPointsWorker>(),
                  "Imgproc::UndistortPoints", info);
}

NAN_METHOD(Imgproc::UndistortPointsAsync) {
  FF::AsyncBindingBase(std::make_shared<ImgprocBindings::UndistortPointsWorker>(),
                   "Imgproc::UndistortPointsAsync", info);
}

NAN_METHOD(Imgproc::CalcHist) {
	FF::TryCatch tryCatch("Imgproc::CalcHist");
  cv::Mat img, mask = cv::noArray().getMat();
  std::vector<std::vector<float>> _ranges;
  if (
	  Mat::Converter::arg(0, &img, info) ||
	  Mat::Converter::optArg(2, &mask, info)
	) {
	  return tryCatch.reThrow();
  }
  if (!info[1]->IsArray()) {
	  return tryCatch.throwError("expected arg 1 to be an array");
  }
  v8::Local<v8::Array> jsHistAxes = v8::Local<v8::Array>::Cast(info[1]);

  cv::Mat inputImg = img;
  int inputType = CV_MAKETYPE(CV_32F, img.channels());
  if (inputType != img.type()) {
    img.convertTo(inputImg, inputType);
  }

  int dims = jsHistAxes->Length();
  int* channels = new int[dims];
  int* histSize = new int[dims];
  std::vector<float*> ranges;
  // TODO replace old macros
  for (int i = 0; i < dims; ++i) {
    ranges.push_back(new float[dims]);
    v8::Local<v8::Object> jsAxis = Nan::To<v8::Object>((Nan::Get(jsHistAxes, i).ToLocalChecked())).ToLocalChecked();
	if (!FF::hasOwnProperty(jsAxis, "ranges")) {
		return tryCatch.throwError("expected axis object to have ranges property");
	}
	v8::Local<v8::Value> jsRangesVal = Nan::Get(jsAxis, Nan::New("ranges").ToLocalChecked()).ToLocalChecked();
	if (!jsRangesVal->IsArray()) {
		return tryCatch.throwError("expected ranges to be an array");
	}
	v8::Local<v8::Array> jsRanges = v8::Local<v8::Array>::Cast(jsRangesVal);
	if (jsRanges->Length() != 2) {
		return tryCatch.throwError("expected ranges to be an array of length 2");
	}
    ranges.at(i)[0] = FF::DoubleConverter::unwrapUnchecked(Nan::Get(jsRanges, 0).ToLocalChecked());
    ranges.at(i)[1] = FF::DoubleConverter::unwrapUnchecked(Nan::Get(jsRanges, 1).ToLocalChecked());
    int channel, bins;

	if (FF::IntConverter::prop(&channel, "channel", jsAxis) || FF::IntConverter::prop(&bins, "bins", jsAxis)) {
		return tryCatch.reThrow();
	}
    channels[i] = channel;
    histSize[i] = bins;
  }

  cv::MatND hist;
  if (dims == 1) {
    hist = calcHist1(inputImg, mask, channels, histSize, ranges);
  }
  else if (dims == 2) {
    hist = calcHist2(inputImg, mask, channels, histSize, ranges);
  }
  else if (dims == 3) {
    hist = calcHist3(inputImg, mask, channels, histSize, ranges);
  }
  else if (dims == 4) {
    hist = calcHist4(inputImg, mask, channels, histSize, ranges);
  }

  for (int i = 0; i < dims; ++i) {
    delete[] ranges.at(i);
  }
  delete[] channels;
  delete[] histSize;

  int outputType = CV_MAKETYPE(CV_64F, img.channels());
  if (outputType != hist.type()) {
    hist.convertTo(hist, outputType);
  }

  info.GetReturnValue().Set(Mat::Converter::wrap(hist));
}

NAN_METHOD(Imgproc::Plot1DHist) {
	FF::TryCatch tryCatch("Imgproc::Plot1DHist");

	cv::Mat hist, plot;
	cv::Vec3d color;
	int lineType = cv::LINE_8;
	int thickness = 2;
	int shift = 0;

	v8::Local<v8::Object> opts = FF::isArgObject(info, 3) ? info[3]->ToObject(Nan::GetCurrentContext()).ToLocalChecked() : Nan::New<v8::Object>();

	if (Mat::Converter::arg(0, &hist, info) ||
		Mat::Converter::arg(1, &plot, info) ||
		Vec3::Converter::arg(2, &color, info) ||
		(
			(FF::isArgObject(info, 3) && (
				FF::IntConverter::optProp(&lineType, "lineType", opts) ||
				FF::IntConverter::optProp(&thickness, "thickness", opts) ||
				FF::IntConverter::optProp(&shift, "shift", opts)
			)) || (
				FF::IntConverter::optArg(3, &lineType, info) ||
				FF::IntConverter::optArg(4, &thickness, info) ||
				FF::IntConverter::optArg(5, &shift, info)
			)
		)
	){
		return tryCatch.reThrow();
	}

	if (1 != hist.cols) {
		return tryCatch.throwError("hist rows mismatch, expected "
			+ std::to_string(1) + ", have " + std::to_string(hist.cols));
	}
	if (hist.channels() != 1) {
		return tryCatch.throwError("expected hist to be single channeled");
	}

  double binWidth = ((double)plot.cols / (double)hist.rows);
  int plotHeight = (cv::max)(256, plot.rows - 20);
  cv::Mat normHist = hist;
  if (hist.type() != CV_64F) {
    hist.convertTo(normHist, CV_64F);
  }
  cv::normalize(normHist, normHist, 0, 256, cv::NORM_MINMAX);

  for (int i = 1; i < hist.rows; i++) {
    cv::line(
      plot,
      cv::Point(binWidth*(i - 1), plotHeight - normHist.at<double>(i - 1)),
      cv::Point(binWidth*i, plotHeight - normHist.at<double>(i)),
      color,
      thickness,
      lineType,
      shift
    );
  }

  info.GetReturnValue().Set(Mat::Converter::wrap(plot));
}

NAN_METHOD(Imgproc::FitLine) {
	FF::TryCatch tryCatch("Imgproc::FitLine");

  if (!info[0]->IsArray()) {
	  return tryCatch.throwError("expected arg 0 to be an array");
  }
  v8::Local<v8::Array> jsPoints = v8::Local<v8::Array>::Cast(info[0]);

  if (jsPoints->Length() < 2) {
	  return tryCatch.throwError("expected arg0 to be an Array with atleast 2 Points");
  }
  v8::Local<v8::Value> jsPt1 = Nan::Get(jsPoints, 0).ToLocalChecked();
  bool isPoint2 = Point2::hasInstance(jsPt1);
  bool isPoint3 = Point3::hasInstance(jsPt1);
  if (!isPoint2 && !isPoint3) {
	  return tryCatch.throwError("expected arg0 to be an Array containing instances of Point2 or Point3");
  }

  std::vector<cv::Point2d> pts2d;
  std::vector<cv::Point3d> pts3d;
  uint distType;
  double param, reps, aeps;
  if ((
	(isPoint2 && Point2::ArrayConverter::arg(0, &pts2d, info)) ||
	(!isPoint2 && Point3::ArrayConverter::arg(0, &pts3d, info))
	) ||
	FF::UintConverter::arg(1, &distType, info) ||
	FF::DoubleConverter::arg(2, &param, info) ||
	FF::DoubleConverter::arg(3, &reps, info) ||
	FF::DoubleConverter::arg(4, &aeps, info)
	) {
	  return tryCatch.reThrow();
  }

  if (isPoint2) {
    cv::Vec4f lineParams;
    cv::fitLine(pts2d, lineParams, (int)distType, param, reps, aeps);
	info.GetReturnValue().Set(Vec4::Converter::wrap(lineParams));
  }
  else {
    cv::Vec6f lineParams;
    cv::fitLine(pts3d, lineParams, (int)distType, param, reps, aeps);
	v8::Local<v8::Array> jsLineParams = Nan::New<v8::Array>(6);
    for (int i = 0; i < 6; i++) {
		Nan::Set(jsLineParams, i, Nan::New(lineParams[i]));
    }
	info.GetReturnValue().Set(jsLineParams);
  }
}

NAN_METHOD(Imgproc::GetTextSize) {
  FF::SyncBindingBase(
    std::make_shared<ImgprocBindings::GetTextSizeWorker>(),
    "Imgproc::GetTextSize",
    info
  );
}

NAN_METHOD(Imgproc::GetTextSizeAsync) {
  FF::AsyncBindingBase(
    std::make_shared<ImgprocBindings::GetTextSizeWorker>(),
    "Imgproc::GetTextSizeAsync",
    info
  );
}

NAN_METHOD(Imgproc::ApplyColorMap) {
  FF::SyncBindingBase(std::make_shared<ImgprocBindings::ApplyColorMapWorker>(),
                  "Imgproc::ApplyColorMap", info);
}

NAN_METHOD(Imgproc::ApplyColorMapAsync) {
  FF::AsyncBindingBase(std::make_shared<ImgprocBindings::ApplyColorMapWorker>(),
                   "Imgproc::ApplyColorMapAsync", info);
}

#if CV_VERSION_MINOR > 1

NAN_METHOD(Imgproc::Canny) {
	FF::SyncBindingBase(std::make_shared<ImgprocBindings::CannyWorker>(),
		"Imgproc::Canny", info);
}

NAN_METHOD(Imgproc::CannyAsync) {
	FF::AsyncBindingBase(std::make_shared<ImgprocBindings::CannyWorker>(),
		"Imgproc::CannyAsync", info);
}

#endif
