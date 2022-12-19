#ifndef Header_Oni_Scanner_Worker
#define Header_Oni_Scanner_Worker

#include <memory>
#include <vector>

#include "nan.h"
#include "Regex.hpp"
#include "Searcher.hpp"

using ::std::shared_ptr;
using ::std::vector;


class OnigScannerWorker : public Nan::AsyncWorker {

	private:

		shared_ptr<OnigSearcher> searcher;
		shared_ptr<OnigResult> bestResult;

		OnigString * source;

		int charOffset;


	public:

		OnigScannerWorker (
			Nan::Callback * callback,
			vector<shared_ptr<OnigRegExp>> regExps,
			Local<String> v8String,
			int charOffset
		) : Nan::AsyncWorker(callback)
		  , charOffset(charOffset) {

			source = new OnigString(v8String);

			searcher = shared_ptr<OnigSearcher>
				(new OnigSearcher(regExps));
		}

		~OnigScannerWorker (){
			delete source;
		}

		void HandleOKCallback();
		void Execute();

};


#endif
