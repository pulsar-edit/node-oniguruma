#ifndef Header_Oni_String
#define Header_Oni_String


#include <memory>
#include "nan.h"

using ::std::shared_ptr;

using ::v8::Local;
using ::v8::Object;
using ::v8::String;


class OnigString : public node::ObjectWrap {

	private:

		static NAN_METHOD(New);

		Nan::Utf8String utf8Value;

		size_t utf8_length_;
		bool hasMultiByteChars;
		int uniqueId_;


		//	Used when `hasMultiByteChars`

		int
			* utf16OffsetToUtf8 ,
			* utf8OffsetToUtf16 ;

		size_t utf16_length_;


	public:

		static void Init(Local<Object> target);

		explicit OnigString(Local<String> value);

		~OnigString();


		auto ConvertUtf16OffsetToUtf8 ( int ) -> int ;
		auto ConvertUtf8OffsetToUtf16 ( int ) -> int ;


		auto uniqueId () const -> int
			{ return uniqueId_; }

		auto utf8_value() const -> const char *
			{ return * utf8Value; }

		auto utf8_length() const -> size_t
			{ return utf8_length_; }
};

#endif
