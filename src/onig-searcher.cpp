
#include "onig-searcher.hpp"


auto OnigSearcher::Search( OnigString * source , int charOffset )
	-> shared_ptr<OnigResult> {

	int byteOffset = source
		-> ConvertUtf16OffsetToUtf8(charOffset);


	int bestLocation = 0 ,
		index = 0 ;

	shared_ptr<OnigResult> bestResult;


	auto iter = regExps.begin();

	while ( iter < regExps.end() ){

		auto regex = ( * iter ).get();

		auto match = regex
			-> Search(source,byteOffset);

		if( match != NULL && match -> Count() > 0 ){

			int location = match
				-> LocationAt(0);

			if( bestResult == NULL || location < bestLocation ){
				bestLocation = location;
				bestResult = match;
				bestResult -> SetIndex(index);
			}

			if( location == byteOffset )
				break;
		}

		++index;
		++iter;
	}

	return bestResult;
}
