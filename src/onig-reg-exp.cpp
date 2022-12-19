
#include "onig-reg-exp.hpp"

using ::v8::Exception;
using ::v8::String;


OnigRegExp::OnigRegExp ( const string & source )
    : source_(source) ,
      regex_(NULL) {

	lastSearchStrUniqueId = -1;
	lastSearchPosition = -1;
	lastSearchResult = NULL;


	hasGAnchor = false;

	const int length =
		source.size();

	for ( size_t index = 0 ; index < length ; index++ ){

		if( source[ index ] != '\\' )
			continue;

		if( index + 1 >= length )
			continue;

		if( source[ index + 1 ] == 'G'){
			hasGAnchor = true;
			break;
		}

		index++;
	}


	const auto sourceData =
		(const UChar * ) source.data();

	OnigErrorInfo error;

	int status = onig_new(
		& regex_ , sourceData , sourceData + source.length() ,
		ONIG_OPTION_CAPTURE_GROUP , ONIG_ENCODING_UTF8 ,
		ONIG_SYNTAX_DEFAULT , & error );

	if(status == ONIG_NORMAL)
		return;

	UChar errorString[ ONIG_MAX_ERROR_MESSAGE_LEN ];
	onig_error_code_to_str(errorString,status,& error);

	Nan::ThrowError(
		Exception::Error(
			Nan::New<String>(reinterpret_cast<char *>(errorString)).ToLocalChecked()));
}


OnigRegExp::~OnigRegExp (){

	if(regex_)
		onig_free(regex_);
}


auto OnigRegExp::Search ( OnigString * string , int position )
	-> shared_ptr<OnigResult> {


	/*
	 *	Shouldn't use caching, as the regular expression
	 *	targets the current search positiion ( \G )
	 */

	if(hasGAnchor)
		return Search
			(string -> utf8_value(),position,string -> utf8_length());

	if(
		lastSearchStrUniqueId == string -> uniqueId() &&
		lastSearchPosition <= position && (
			lastSearchResult == NULL ||
			lastSearchResult -> LocationAt(0) >= position
		)
	)	return lastSearchResult;


	lastSearchStrUniqueId = string -> uniqueId();
	lastSearchPosition = position;

	lastSearchResult = Search
		(string -> utf8_value(),position,string -> utf8_length());

	return lastSearchResult;
}


auto OnigRegExp::Search (
	const char * data ,
	size_t position ,
	size_t end
) -> shared_ptr<OnigResult> {

	if( ! regex_ ){

		Nan::ThrowError(
			Exception::Error(
				Nan::New<String>("RegExp is not valid").ToLocalChecked()));

		return shared_ptr<OnigResult>();
	}

	const auto searchData =
		reinterpret_cast<const UChar*>(data);

	auto region = onig_region_new();

	int status = onig_search(
		regex_ , searchData , searchData + end ,
		searchData + position , searchData + end ,
		region , ONIG_OPTION_NONE );

	if( status == ONIG_MISMATCH ){
		onig_region_free(region, 1);
		return shared_ptr<OnigResult>();
	}

	return shared_ptr<OnigResult>
		(new OnigResult(region,-1));
}
