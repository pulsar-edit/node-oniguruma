
#include "Scanner-Worker.hpp"
#include "Scanner.hpp"


using ::v8::FunctionTemplate;
using ::v8::Context;


void OnigScanner::Init ( Local<Object> target ){

	const auto classname = Nan::New<String>("OnigScanner")
		.ToLocalChecked();

	auto model = Nan::New<FunctionTemplate>(OnigScanner::New);
	model -> SetClassName(classname);

	model
		-> InstanceTemplate()
		-> SetInternalFieldCount(1);

	auto context = Nan::GetCurrentContext();
	auto isolate = context -> GetIsolate();


	const auto findAsync = Nan::New<FunctionTemplate>
		(OnigScanner::FindNextMatch);

	model
		-> PrototypeTemplate()
		-> Set(isolate,"_findNextMatch",findAsync);


	const auto findSync = Nan::New<FunctionTemplate>
		(OnigScanner::FindNextMatchSync);

	model
		-> PrototypeTemplate()
		-> Set(isolate,"_findNextMatchSync",findSync);


	const auto function = model
		-> GetFunction(context)
		.  ToLocalChecked();

	Nan::Set(target,classname,function);
}


void InitModule ( Local<Object> target ){
	OnigScanner::Init(target);
	OnigString::Init(target);
}


NODE_MODULE(onig_scanner,InitModule)


NAN_METHOD( OnigScanner::New ){

	Nan::HandleScope scope;

	auto scanner = new OnigScanner(Local<Array>::Cast(info[0]));
	scanner -> Wrap(info.This());

	info.GetReturnValue().SetUndefined();
}


NAN_METHOD( OnigScanner::FindNextMatchSync ){

	Nan::HandleScope scope;

	auto scanner = node::ObjectWrap
		::Unwrap<OnigScanner>(info.This());

	auto param1 = Local<Object>
		::Cast(info[0]);

	auto param2 = Local<Number>
		::Cast(info[1]);

	Local<Value> result;

	if( param1 -> IsString() ){

		auto v8String = Local<String>
			::Cast(info[0]);
		result = scanner -> FindNextMatchSync(v8String,param2);

	} else {

		auto onigString = node::ObjectWrap
			::Unwrap<OnigString>(info[0].As<Object>());

		result = scanner -> FindNextMatchSync(onigString,param2);
	}

	info
	.GetReturnValue()
	.Set(result);
}


NAN_METHOD( OnigScanner::FindNextMatch ){

	Nan::HandleScope scope;

	auto scanner = node::ObjectWrap
		::Unwrap<OnigScanner>(info.This());

	scanner -> FindNextMatch(
		Local<String>::Cast(info[0]) ,
		Local<Number>::Cast(info[1]) ,
		Local<Function>::Cast(info[2]) );

	info
	.GetReturnValue()
	.SetUndefined();
}


OnigScanner::OnigScanner ( Local<Array> sources ){

	int length = sources -> Length();
	regExps.resize(length);

	auto context = Nan::GetCurrentContext();

	for (int i = 0; i < length; i++){

		Nan::Utf8String utf8Value(sources -> Get(context,i).ToLocalChecked());

		auto regex = new OnigRegExp(string( * utf8Value ));

		regExps[i] = shared_ptr<OnigRegExp>(regex);
	}

	searcher = shared_ptr<OnigSearcher>
		(new OnigSearcher(regExps));
}


OnigScanner::~OnigScanner (){}


void OnigScanner::FindNextMatch (
	Local<String> v8String ,
	Local<Number> v8StartLocation ,
	Local<Function> v8Callback
){

	int charOffset = v8StartLocation
		-> Value();

	auto callback = new Nan
		::Callback(v8Callback);

	auto worker = new OnigScannerWorker
		( callback , regExps , v8String , charOffset );

	Nan::AsyncQueueWorker(worker);
}


auto OnigScanner::FindNextMatchSync (
	Local<String> v8String ,
	Local<Number> v8StartLocation
) -> Local<Value> {

	auto source = new OnigString(v8String);

	auto match = FindNextMatchSync(source,v8StartLocation);

	delete source;
	return match;
}


auto OnigScanner::FindNextMatchSync (
	OnigString * source ,
	Local<Number> v8StartLocation
) -> Local<Value> {

	int charOffset = v8StartLocation
		-> Value();

	auto bestResult = searcher
		-> Search(source, charOffset);

	if( bestResult == NULL )
		return Nan::Null();

	auto result = Nan::New<Object>();

	Nan::Set( result ,
		Nan::New<String>("index").ToLocalChecked() ,
		Nan::New<Number>(bestResult -> Index()) );

	Nan::Set( result ,
		Nan::New<String>("captureIndices").ToLocalChecked() ,
		CaptureIndicesForMatch(bestResult.get(),source) );

	return result;
}


auto OnigScanner::CaptureIndicesForMatch (
	OnigResult * result ,
	OnigString * source
) -> Local<Value> {

	int resultCount = result -> Count();

	auto captures = Nan::New<Array>(resultCount);

	for ( int index = 0 ; index < resultCount ; index++ ){

		int
			captureStart = source
				-> ConvertUtf8OffsetToUtf16(result -> LocationAt(index)),

			captureEnd = source
				-> ConvertUtf8OffsetToUtf16(result -> LocationAt(index) + result -> LengthAt(index));

		auto  capture = Nan::New<Object>();

		Nan::Set( capture ,
			Nan::New<String>("index").ToLocalChecked() ,
			Nan::New<Number>(index) );

		Nan::Set( capture ,
			Nan::New<String>("start").ToLocalChecked() ,
			Nan::New<Number>(captureStart) );

		Nan::Set( capture ,
			Nan::New<String>("end").ToLocalChecked() ,
			Nan::New<Number>(captureEnd) );

		Nan::Set( capture ,
			Nan::New<String>("length").ToLocalChecked() ,
			Nan::New<Number>(captureEnd - captureStart) );

		Nan::Set( captures , index, capture);
	}

	return captures;
}
