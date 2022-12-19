#ifndef Header_Oni_Scanner
#define Header_Oni_Scanner

#include "nan.h"
#include "String.hpp"
#include "Result.hpp"
#include "Regex.hpp"
#include "Searcher.hpp"

using ::std::shared_ptr;
using ::std::vector;

using ::v8::Array;
using ::v8::Function;
using ::v8::Local;
using ::v8::Number;
using ::v8::Object;
using ::v8::String;
using ::v8::Value;




class OnigScanner : public node::ObjectWrap {

	private:

		vector<shared_ptr<OnigRegExp>> regExps;
		shared_ptr<OnigSearcher> searcher;

		static auto CaptureIndicesForMatch (
			OnigResult * result ,
			OnigString * source )
			-> Local<Value> ;

		static NAN_METHOD(FindNextMatchSync);
		static NAN_METHOD(FindNextMatch);
		static NAN_METHOD(New);

		explicit OnigScanner ( Local<Array> sources );

		~OnigScanner ();

		auto FindNextMatchSync (
			OnigString * onigString ,
			Local<Number> v8StartLocation )
			-> Local<Value> ;

		auto FindNextMatchSync (
			Local<String> v8String ,
			Local<Number> v8StartLocation )
			-> Local<Value> ;

		void FindNextMatch (
			Local<String> v8String ,
			Local<Number> v8StartLocation ,
			Local<Function> v8Callback );


	public:

		static void Init ( Local<Object> target );

};

#endif
