#include "macros.h"
#include "CvBinding.h"
#include "../FeatureDetector.h"

#ifndef __FF_ORBDETECTOR_H__
#define __FF_ORBDETECTOR_H__


class ORBDetector : public FeatureDetector, public FF::ObjectWrapTemplate<ORBDetector, cv::Ptr<cv::ORB>> {
public:
	static Nan::Persistent<v8::FunctionTemplate> constructor;

	static const char* getClassName() {
		return "ORBDetector";
	}

	cv::Ptr<cv::FeatureDetector> getDetector() {
		return self;
	}

	FF_GETTER_CUSTOM(maxFeatures, FF::IntConverter, self->getMaxFeatures());
	FF_GETTER_CUSTOM(scaleFactor, FF::FloatConverter, self->getScaleFactor());
	FF_GETTER_CUSTOM(nLevels, FF::IntConverter, self->getNLevels());
	FF_GETTER_CUSTOM(edgeThreshold, FF::IntConverter, self->getEdgeThreshold());
	FF_GETTER_CUSTOM(firstLevel, FF::IntConverter, self->getFirstLevel());
	FF_GETTER_CUSTOM(WTA_K, FF::IntConverter, self->getWTA_K());
	FF_GETTER_CUSTOM(scoreType, FF::IntConverter, self->getScoreType());
	FF_GETTER_CUSTOM(patchSize, FF::IntConverter, self->getPatchSize());
	FF_GETTER_CUSTOM(fastThreshold, FF::IntConverter, self->getFastThreshold());

	static NAN_MODULE_INIT(Init);
	static NAN_METHOD(New);

	class NewBinding : public CvBinding {
	public:
		void construct(Nan::NAN_METHOD_ARGS_TYPE info) {
			FF::TryCatch tryCatch("SURFDetector::New");
			FF_ASSERT_CONSTRUCT_CALL();

			auto maxFeatures = opt<FF::IntConverter>("maxFeatures", 500);
			auto scaleFactor = opt<FF::DoubleConverter>("scaleFactor", (double)1.2f);
			auto nLevels = opt<FF::IntConverter>("nLevels", 8);
			auto edgeThreshold = opt<FF::IntConverter>("edgeThreshold", 31);
			auto firstLevel = opt<FF::IntConverter>("firstLevel", 0);
			auto WTA_K = opt<FF::IntConverter>("WTA_K", 2);
			auto scoreType = opt<FF::IntConverter>("scoreType", cv::ORB::HARRIS_SCORE);
			auto patchSize = opt<FF::IntConverter>("patchSize", 31);
			auto fastThreshold = opt<FF::IntConverter>("fastThreshold", 20);

			if (applyUnwrappers(info)) {
				return tryCatch.reThrow();
			}

			ORBDetector* self = new ORBDetector();
			self->setNativeObject(cv::ORB::create(
				maxFeatures->ref(), 
				scaleFactor->ref(),
				nLevels->ref(),
				edgeThreshold->ref(),
				firstLevel->ref(),
				WTA_K->ref(),
				scoreType->ref(),
				patchSize->ref(),
				fastThreshold->ref()
			));
			self->Wrap(info.Holder());
			info.GetReturnValue().Set(info.Holder());
		};
	};
};

#endif