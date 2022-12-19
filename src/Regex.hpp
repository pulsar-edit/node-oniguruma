#ifndef Header_Oni_Regex
#define Header_Oni_Regex

#include <memory>
#include <string>

#include "Result.hpp"
#include "String.hpp"

using ::std::shared_ptr;
using ::std::string;


class OnigRegExp {

	private:

		shared_ptr<OnigResult> lastSearchResult;

		int lastSearchStrUniqueId ,
			lastSearchPosition ;

		bool hasGAnchor;

		string source_;
		regex_t * regex_;


		auto Search ( const char * data , size_t position , size_t end )
			-> shared_ptr<OnigResult> ;


		// Disallow copying

		OnigRegExp & operator = ( const OnigRegExp & );

		OnigRegExp ( const OnigRegExp & );


	public:

		explicit OnigRegExp ( const string & source );

		~OnigRegExp();

		auto Search ( OnigString * , int position )
			-> shared_ptr<OnigResult> ;

};

#endif
