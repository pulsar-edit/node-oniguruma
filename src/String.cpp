
#include "String.hpp"

using ::v8::FunctionTemplate;
using ::v8::Context;


void OnigString::Init ( Local<Object> target ){

	Local<FunctionTemplate> model =
		Nan::New<FunctionTemplate>(OnigString::New);

	auto classname = Nan::New<String>("OnigString")
		.ToLocalChecked();

	model -> SetClassName(classname);

	model
		-> InstanceTemplate()
		-> SetInternalFieldCount(1);

	auto context = Nan::GetCurrentContext();

	auto function = model
		-> GetFunction(context)
		.  ToLocalChecked();

	Nan::Set(target,classname,function);
}


NAN_METHOD( OnigString::New ){

	auto content = Local<String>::Cast(info[0]);

	if( ! content -> IsString() ){
		Nan::ThrowTypeError("Argument must be a string");
		return;
	}

	auto string = new OnigString(content);

	string -> Wrap(info.This());

	Nan::Set(
		info.This() ,
		Nan::New("content")
			.ToLocalChecked() ,
		content
	);
}


OnigString::OnigString( Local<String> value )
	: utf8Value(value)
	, utf8_length_(utf8Value.length()){

	static int idGenerator = 0;
	uniqueId_ = ++idGenerator;

	hasMultiByteChars = ((size_t) value -> Length() != utf8_length_);

	if( ! hasMultiByteChars )
		return;

	#if NODE_MODULE_VERSION > 48
		auto isolate = v8::Isolate::GetCurrent();
		String::Value utf16Value(isolate,value);
	#else
		String::Value utf16Value(value);
	#endif

	utf16_length_ = utf16Value.length();

	utf16OffsetToUtf8 = new int[utf16_length_ + 1];
	utf16OffsetToUtf8[utf16_length_] = utf8_length_;

	utf8OffsetToUtf16 = new int[utf8_length_ + 1];
	utf8OffsetToUtf16[utf8_length_] = utf16_length_;


	// http://stackoverflow.com/a/148766

	const int length = utf16_length_;

	int i8 = 0;


	for( int i16 = 0 ; i16 < length ; i16++ ){

		uint16_t in = ( * utf16Value )[ i16 ];

		unsigned int codepoint = in;
		bool wasSurrogatePair = false;


		// Hit a high surrogate, try to look for a matching low surrogate

		if( in >= 0xd800 && in <= 0xdbff ){


			if( i16 + 1 < length ){

				uint16_t next = ( * utf16Value )[ i16 + 1 ];

				// Found the matching low surrogate

				if( next >= 0xdc00 && next <= 0xdfff ){
					wasSurrogatePair = true;
					codepoint = (((in - 0xd800) << 10) + 0x10000) | (next - 0xdc00);
				}
			}
		}

		utf16OffsetToUtf8[i16] = i8;

		if( codepoint <= 0x7f ){
			utf8OffsetToUtf16[i8++] = i16;
		} else
		if( codepoint <= 0x7ff ){
			utf8OffsetToUtf16[i8++] = i16;
			utf8OffsetToUtf16[i8++] = i16;
		} else
		if( codepoint <= 0xffff ){
			utf8OffsetToUtf16[i8++] = i16;
			utf8OffsetToUtf16[i8++] = i16;
			utf8OffsetToUtf16[i8++] = i16;
		} else {
			utf8OffsetToUtf16[i8++] = i16;
			utf8OffsetToUtf16[i8++] = i16;
			utf8OffsetToUtf16[i8++] = i16;
			utf8OffsetToUtf16[i8++] = i16;
		}

		if( wasSurrogatePair ){

			utf16OffsetToUtf8[i16 + 1] =
				utf16OffsetToUtf8[i16];

			i16++;
		}
	}
}


OnigString::~OnigString (){

	if( ! hasMultiByteChars )
		return;

	delete [] utf16OffsetToUtf8;
	delete [] utf8OffsetToUtf16;
}


int OnigString::ConvertUtf8OffsetToUtf16 ( int offset ){

	if( ! hasMultiByteChars )
		return offset;

	if( offset < 0 )
		return 0;

	return ( (size_t) offset <= utf8_length_ )
		? utf8OffsetToUtf16[ offset ]
		: utf16_length_ ;
}


int OnigString::ConvertUtf16OffsetToUtf8 ( int offset ){

	if( ! hasMultiByteChars )
		return offset;

	if( offset < 0 )
		return 0;

	return ( (size_t) offset <= utf16_length_ )
		? utf16OffsetToUtf8[ offset ]
		: utf8_length_ ;
}
